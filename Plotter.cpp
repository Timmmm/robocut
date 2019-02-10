#include "Plotter.h"

#include <libusb/libusb.h>

#include <iostream>
#include <cmath>

namespace
{
int VENDOR_ID = 0; //ProgramOptions::Instance().getVendorUSB_ID();
int PRODUCT_ID = 0; //ProgramOptions::Instance().getProductUSB_ID();

std::string UsbError(int e)
{
	switch (e)
	{
	case LIBUSB_SUCCESS:
		return "Success (internal error)";
	case LIBUSB_ERROR_IO:
		return "I/O error";
	case LIBUSB_ERROR_INVALID_PARAM:
		return "Invalid parameter";
	case LIBUSB_ERROR_ACCESS:
		return "Access denied";
	case LIBUSB_ERROR_NO_DEVICE:
		return "No device";
	case LIBUSB_ERROR_NOT_FOUND:
		return "Entity not found";
	case LIBUSB_ERROR_BUSY:
		return "Resource busy";
	case LIBUSB_ERROR_TIMEOUT:
		return "Timeout";
	case LIBUSB_ERROR_OVERFLOW:
		return "Overflow";
	case LIBUSB_ERROR_PIPE:
		return "Pipe error";
	case LIBUSB_ERROR_INTERRUPTED:
		return "Syscall interrupted";
	case LIBUSB_ERROR_NO_MEM:
		return "Insufficient memory";
	case LIBUSB_ERROR_NOT_SUPPORTED:
		return "Operation not supported";
	case LIBUSB_ERROR_OTHER:
		return "Other error";
	}
	return "Unknown error";
}

// Send some data to the USB device.
Error UsbSend(libusb_device_handle* handle, const string& s, int timeout = 0)
{
	// The manual for the Silhouette Portrait indicate that some plotters will
	// produce skewed and distorted results (only really perceptible when you
	// try to use registration marks). One of the solutions in their manual is
	// to change the configuration in the Silhouette Studio to send 500 bytes at
	// a time, considering we are probably very far away from saturating the
	// USB communication while sending commands to the plotter, I take a far more
	// conservative approach of using the same size as the receiving block.
	for (unsigned int i = 0; i < s.length(); i += 64)
	{
		std::string data = s.substr(i, 64);
		int transferred;
		unsigned char endpoint = 0x01;
		int ret = libusb_bulk_transfer(handle,
		                               endpoint,
		                               reinterpret_cast<unsigned char*>(const_cast<char*>(data.c_str())),
		                               static_cast<int>(data.length()),
		                               &transferred,
		                               timeout);
		if (ret != 0)
		{
			cerr << "Error writing to device: " << UsbError(ret) << endl;
			return Error("Error writing to device: " + UsbError(ret));
		}
		if (transferred < 0 || static_cast<std::size_t>(transferred) != data.length())
		{
			cerr << "Warning, some data not transferred correctly." << endl;
			return Err("Some data not transfered. Attempted: " + ULLtoS(data.length()) + " Transferred: " + ItoS(transferred));
		}
	}
	return Success;
}

// Receive some data up to the maximum packet size of 64. Because we use the packet size
// there can never be overflows, and 64 is ok because none of the craft robo responses are
// anywhere near that long.
Error UsbReceive(libusb_device_handle* handle, string& s, int timeout = 0)
{
	s.clear();

	// A buffer that is one packet long.
	const int PacketSize = 64;
	unsigned char buffer[PacketSize];
	int transferred = 0;
	unsigned char endpoint = 0x82;
	int ret = libusb_bulk_transfer(handle, endpoint, buffer, PacketSize, &transferred, timeout);
	if (ret != 0) // But it could be a timout.
	{
		cerr << "Error reading from device: " << UsbError(ret) << endl;
		return Error("Error reading from device: " + UsbError(ret));
	}

	s = string(reinterpret_cast<char*>(buffer), transferred);
	return Success;
}

// Send a control message
//Error UsbControl(libusb_device_handle* handle, ...)
//{
//	libusb_control_transfer(handle,
//		uint8_t request_type, uint8_t request, uint16_t value, uint16_t index,
//		unsigned char *data, uint16_t length, unsigned int timeout);
//}

libusb_device_handle* UsbOpen(CutterId* id)
{
	libusb_device** list;
	ssize_t cnt = libusb_get_device_list(nullptr, &list);
	if (cnt < 1)
	{
		id->msg = Error("Couldn't get usb device list.");
		return nullptr;
	}

	int err = 0;

	libusb_device* found = nullptr;
	for (ssize_t i = 0; i < cnt; ++i)
	{
		libusb_device* device = list[i];
		libusb_device_descriptor desc;
		libusb_get_device_descriptor(device, &desc);
		
		// I don't want to be more specific than this really.
		if (desc.idVendor != VENDOR_ID && desc.idVendor != VENDOR_ID_GRAPHTEC)
			continue;
		
		if (desc.idProduct != PRODUCT_ID && PRODUCT_ID_LIST.count(desc.idProduct) == 0)
			continue;
		
		// Just use the first one. Who has two?!
		found = device;
		id->usb_product_id = desc.idProduct;
		id->usb_vendor_id = desc.idVendor;
		break;
	}

	if (!found)
	{
		libusb_free_device_list(list, 1);
		id->msg = Error("Couldn't find supported device. Is it connected to the system and powered "
		                "on? The current list of supported devices includes the CC200, CC300, "
		                "Silhouette SD 1, SD 2, Cameo, Cameo 3 and Portrait.");
		return nullptr;
	}

	libusb_device_handle* handle;

	err = libusb_open(found, &handle);
	if (err != 0)
	{
		libusb_free_device_list(list, 1);
		id->msg = Error("Error accessing Craft Robo 2: " + UsbError(err) + ". Do you have permission (on Linux make sure you are in the group 'lp').");
		return nullptr;
	}

	libusb_free_device_list(list, 1);
	id->msg = "Ready";
	return handle;
}

// caller can use UsbSend() afterwards, and should
// finally do libusb_release_interface(handle, 0); libusb_close(handle);
libusb_device_handle *UsbInit(struct CutterId *id)
{
	// Now do stuff with the handle.
	int r = 0;

	libusb_device_handle* handle = UsbOpen(id);
	if (!handle) return nullptr;

	if (libusb_kernel_driver_active(handle, 0) == 1)
	{
		r = libusb_detach_kernel_driver(handle, 0);
		if (r != 0)
		{
			libusb_close(handle);
			id->msg = Error("Error detaching kernel USB driver: " + UsbError(r));
			return nullptr;
		}
	}

	r = libusb_reset_device(handle);
	if (r != 0)
	{
		libusb_close(handle);
		id->msg = Error("Error resetting device: " + UsbError(r));
		return nullptr;
	}

	std::cout << "Selecting configuration.\n";
	r = libusb_set_configuration(handle, 1);
	if (r < 0)
	{
		libusb_close(handle);
		id->msg = "Error setting USB configuration: " + UsbError(r);
		return nullptr;
	}


	std::cout << "Claiming main control interface.\n";
	r = libusb_claim_interface(handle, 0);
	if (r < 0)
	{
		libusb_close(handle);
		id->msg = "Error claiming USB interface: " + UsbError(r);
		return nullptr;
	}

	std::cout << "Setting alt interface.\n";
	r = libusb_set_interface_alt_setting(handle, 0, 0); // Probably not really necessary.
	if (r < 0)
	{
		libusb_close(handle);
		id->msg = "Error setting alternate USB interface: " + UsbError(r);
		return nullptr;
	}

	std::cout << "Initialisation successful.\n";
	return handle;
}

/*
QList<QPolygonF> Transform_Silhouette_Cameo(QList<QPolygonF> cuts, double *mediawidth, double *mediaheight)
{
	const double devicewidth = 300;		// cameo is 12inch aka 300mm wide
	double w;
	double h = *mediaheight;
	
	std::cout << "Transform_Silhouette_Cameo " << *mediawidth << "," << *mediaheight << "\n";
//	if (0) // TODO: ??
//	{
//		w = *mediawidth;
//		std::cout << "Paper right aligned\n";
//	}
//	else
//	{
		w = devicewidth;
		// adjust the used media area, so that the hardware does not clip.
		*mediawidth = devicewidth;
		std::cout << "Paper left aligned\n";
//	}
	
	
	// flip it around 180 deg, and go backwards through the path list.
	QList<QPolygonF> paths;
	for (int i = cuts.size()-1; i >= 0; i--)
	{
		QPolygonF poly;
		for (int j = 0; j < cuts[i].size(); j++)
		{
			// QPolygonF is a QList<QPointF>
			double x = cuts[i][j].x();
			double y = cuts[i][j].y();
			poly << QPointF(w-x, h-y);
		}
		paths << poly;
	}
	return paths;
}
*/
} // anonymous namespace

Error Cut(CutParams p)
{
	VENDOR_ID = 0;//ProgramOptions::Instance().getVendorUSB_ID();
	PRODUCT_ID = 0;//ProgramOptions::Instance().getProductUSB_ID();
	
	std::cout << "Cutting... VENDOR_ID : " << VENDOR_ID << " PRODUCT_ID: " << PRODUCT_ID
		 << " mediawidth: " << p.mediawidth << " mediaheight: " << p.mediaheight
		 << "media: " << p.media << " speed: " << p.speed << " pressure: " << p.pressure
		 << " trackenhancing: " << p.trackenhancing << " regmark: " <<  p.regmark
	     << " regsearch:" <<  p.regsearch <<" regwidth:" <<  p.regwidth << " reglength: " << p.regheight << "\n";

	if (p.media < 100 || p.media > 300)
		p.media = 300;
	if (p.speed < 1)
		p.speed = 1;
	if (p.speed > 10)
		p.speed = 10;
	if (p.pressure < 1)
		p.pressure = 1;
	if (p.pressure > 33)
		p.pressure = 33;

	// how can VENDOR_ID / PRODUCT_ID report the correct values abve???
	struct CutterId id = { "?", 0, 0 };
	libusb_device_handle* handle = UsbInit(&id);
	
	// TODO: Renable this.
//	if (id.usb_vendor_id == VENDOR_ID_GRAPHTEC &&
//	        (id.usb_product_id == PRODUCT_ID_SILHOUETTE_CAMEO ||
//	         id.usb_product_id == PRODUCT_ID_SILHOUETTE_CAMEO_3))
//	{
//		// should this also transform the regwidth regheigth or not?
//		p.cuts = Transform_Silhouette_Cameo(p.cuts, &p.mediawidth, &p.mediaheight);
//	}

	// TODO: Use Rust-style error handling?

	Error e = Success;
	string resp;

	// Initialise plotter.
	e = UsbSend(handle, "\x1b\x04");
	if (!e) goto error;

	// Status request.
	e = UsbSend(handle, "\x1b\x05");
	if (!e) goto error;

	e = UsbReceive(handle, resp, 5000);
	if (!e) goto error;

	if (resp != "0\x03") // 0 = Ready. 1 = Moving. 2 = Nothing loaded. "  " = ??
	{
		if (resp == "1\x03")
		  e = Error("Moving, please try again.");
		else if (resp == "2\x03")
		  e = Error("Empty tray, please load media.");	// Silhouette Cameo
		else
		  e = Error("Invalid response from plotter: " + resp);
		goto error;
	}

	// Home the cutter.
	e = UsbSend(handle, "TT\x03");
	if (!e) goto error;

	// Query version.
	e = UsbSend(handle, "FG\x03");
	if (!e) goto error;

	// Receive the firmware version.
	e = UsbReceive(handle, resp, 10000); // Large timeout because the plotter moves.
	if (!e) goto error;
	// Don't really care about this.
//	if (resp.length() != 10)
//	{
//		e = Error("Version error: " + version);
//		goto error;
//	}

	e = UsbSend(handle, "FW" + ItoS(p.media) + "\x03");
	if (!e) goto error;

	e = UsbSend(handle, "!" + ItoS(p.speed) + "\x03");
	if (!e) goto error;

	e = UsbSend(handle, "FX" + ItoS(p.pressure) + "\x03");
	if (!e) goto error;

	// I think this sets the distance from the position of the plotter
	// head to the actual cutting point, maybe in 0.1 mms (todo: Measure blade).
	// It is 0 for the pen, 18 for cutting.
	// C possible stands for curvature. Not that any of the other letters make sense...
	e = UsbSend(handle, "FC" + ItoS(p.media == 113 ? 0 : 18) + "\x03");
	if (!e) goto error;

	e = UsbSend(handle, "FY" + ItoS(p.trackenhancing ? 0 : 1) + "\x03");
	if (!e) goto error;

	// Set to portrait. FN1 does landscape but it's easier just to rotate the image.
	e = UsbSend(handle, "FN0\x03");
	if (!e) goto error;

	e = UsbSend(handle, "FE0\x03"); // No idea what this does.
	if (!e) goto error;

	e = UsbSend(handle, "TB71\x03"); // Again, no idea. Maybe something to do with registration marks?
	if (!e) goto error;

	e = UsbReceive(handle, resp, 10000); // Allow 10s. Seems reasonable.
	if (!e) goto error;

	if (resp != "    0,    0\x03")
	{
		e = Error("Invalid response from plotter.");
		goto error;
	}


	// Begin page definition.
	e = UsbSend(handle, "FA\x03");
	if (!e) goto error;


	// Block for all the "jump to error crosses initialization" errors. Really need to use exceptions!
	{
		// Page size: height,width in 20ths of a mm minus a margin. This is for A4. TODO: Find maximum and use that.
		std::stringstream page;

		int width = static_cast<int>(lround(p.mediawidth * 20.0));
		int height = static_cast<int>(lround(p.mediaheight * 20.0));

		int margintop = 0;//ProgramOptions::Instance().getMarginTop();
		int marginright = 0;//ProgramOptions::Instance().getMarginRight();

		e = UsbSend(handle, "FU" + ItoS(height - margintop) + "," + ItoS(width - marginright) + "\x03");
		if (!e) goto error;
		e = UsbSend(handle, "FM1\x03"); // ?
		if (!e) goto error;

		if (p.regmark)
		{
			std::stringstream regmarkstr;
			regmarkstr.precision(0);
			std::string searchregchar = "23,";
			int regw = static_cast<int>(lround(p.regwidth * 20.0));
			int regl = static_cast<int>(lround(p.regheight * 20.0));
			e = UsbSend(handle, "TB50,381\x03"); //only with registration (it was TB50,1) ???
			if (!e) goto error;
			
			if (p.regsearch)
				searchregchar ="123,";
			
			regmarkstr <<  "TB99\x03TB55,1\x03TB" + searchregchar + ItoS(regw) + "," + ItoS(regl) + "\x03";
			
			std::cout << "Registration mark string: " << regmarkstr.str() << "\n";
			
			e = UsbSend(handle, regmarkstr.str()); //registration mark test /1-2: 180.0mm / 1-3: 230.0mm (origin 15mmx20mm)
			if (!e) goto error;
			
			e = UsbSend(handle, "FQ5\x03"); // only with registration ???
			if (!e) goto error; 

			e = UsbReceive(handle, resp, 40000); // Allow 20s for reply...
						if (!e) goto error;
			if (resp != "    0,    0\x03")
			{
				e = Error("Couldn't find registration marks.");
				std::cout << resp << "\n";
				goto error;
			}
// Looks like if the reg marks work it gets 3 messages back (if it fails it times out because it only gets the first message)
			e = UsbReceive(handle, resp, 40000); // Allow 20s for reply...
			if (!e) goto error;
			if (resp != "    0\x03")
			{
				e = Error("Couldn't find registration marks.");
				std::cout << resp << "\n";
				goto error;
			}

			e = UsbReceive(handle, resp, 40000); // Allow 20s for reply...
			if (!e) goto error;
			if (resp != "    1\x03")
			{
				e = Error("Couldn't find registration marks.");
				std::cout << resp << "\n";
				goto error;
			}
		}
		else
		{
			e = UsbSend(handle, "TB50,1\x03"); // ???
			if (!e) goto error;
		}

		if (!e) goto error;
		// I think this is the feed command. Sometimes it is 5588 - maybe a maximum?
		e = UsbSend(handle, "FO" + ItoS(height - margintop) + "\x03");
		if (!e) goto error;

		page.flags(std::ios::fixed);
		page.precision(0);
		page << "&100,100,100,\\0,0,Z" << ItoS(width) << "," << ItoS(height) << ",L0";
		for (int i = 0; i < p.cuts.size(); ++i)
		{
			if (p.cuts[i].size() < 2)
				continue;

			double x = p.cuts[i][0].x()*20.0;
			double y = p.cuts[i][0].y()*20.0;

			double xorigin = 0;//ProgramOptions::Instance().getRegOriginWidthMM();
			double yorigin = 0;//ProgramOptions::Instance().getRegOriginHeightMM();

			if (p.regmark)
			{
				x = x - (xorigin*20.0);
				y = y + (yorigin*20.0);
			}

			if (x < 0.0) x = 0.0;
			if (x > width) x = width;

			if (y < 0.0) y = 0.0;
			if (y > height) y = height;

			page << ",M" << x << "," << height-y;
			for (int j = 1; j < p.cuts[i].size(); ++j)
			{
				x = p.cuts[i][j].x()*20.0;
				y = p.cuts[i][j].y()*20.0;

				if (p.regmark)
				{
					x = x - (xorigin*20.0);
					y = y + (yorigin*20.0);
				}
				
				bool draw = true;
				if (x <= 0.0) 
				{
					x = 0.0;
					draw = false;
				}
				if (x >= width)
				{
					x = width;
					draw = false;
				}
				if (y <= 0.0)
				{ 
					y = 0.0;
					draw = false;
				}
				if (y >= height) 
				{
					y = height;
					draw = false;
				}
				
				if (draw) page << ",D" << x << "," << height-y;
				else page << ",M" << x << "," << height-y; // if outside the range just move
			}
		}


		page << "&1,1,1,TB50,0\x03"; // TB maybe .. ah I dunno. Need to experiment. No idea what &1,1,1 does either.

		// std::cout << page.str() << "\n";

		e = UsbSend(handle, page.str());
		if (!e) goto error;
	}

	// Feed the page out.
	e = UsbSend(handle, "FO0\x03");
	if (!e) goto error;

	// Halt?
	e = UsbSend(handle, "H,");
	if (!e) goto error;


	// Don't really care about the results of these!
	libusb_release_interface(handle, 0);
	libusb_close(handle);

	return Success;

error: // Hey, this is basically C and I can't be bothered to properly C++-ify it. TODO: Use exceptions.
	cout << "Error: " << e << endl;
	libusb_release_interface(handle, 0);
	libusb_close(handle);
	return e;

}

