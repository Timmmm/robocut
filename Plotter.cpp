#include "Plotter.h"

#include <libusb/libusb.h>

#include <iostream>
#include <cmath>

namespace
{

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

struct DeviceHandleDeleter
{
	void operator()(libusb_device_handle* handle) const
	{
		// Release the interface if we have claimed it. This will return an
		// error if we haven't claimed it but we can just ignore it.
		// TODO: Do this more cleanly (I should really make a proper device class).
		libusb_release_interface(handle, 0);
		libusb_close(handle);
	}
};

using device_handle = std::unique_ptr<libusb_device_handle, DeviceHandleDeleter>;

// Send some data to the USB device.
SResult<> UsbSend(const device_handle& handle, const std::string& s, unsigned int timeout = 0)
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
		int ret = libusb_bulk_transfer(handle.get(),
		                               endpoint,
		                               reinterpret_cast<unsigned char*>(const_cast<char*>(data.c_str())),
		                               static_cast<int>(data.length()),
		                               &transferred,
		                               timeout);
		if (ret != 0)
		{
			return Err("Error writing to device: " + UsbError(ret));
		}
		if (transferred < 0 || static_cast<std::size_t>(transferred) != data.length())
		{
			return Err("Some data not transfered. Attempted: " + ULLtoS(data.length()) + " Transferred: " + ItoS(transferred));
		}
	}
	return Ok();
}

// Receive some data up to the maximum packet size of 64. Because we use the packet size
// there can never be overflows, and 64 is ok because none of the craft robo responses are
// anywhere near that long.
SResult<std::string> UsbReceive(const device_handle& handle, unsigned int timeout = 0)
{
	// A buffer that is one packet long.
	const int PacketSize = 64;
	unsigned char buffer[PacketSize];
	int transferred = 0;
	unsigned char endpoint = 0x82;
	int ret = libusb_bulk_transfer(handle.get(), endpoint, buffer, PacketSize, &transferred, timeout);
	if (ret != 0) // But it could be a timout.
	{
		return Err("Error reading from device: " + UsbError(ret));
	}

	return Ok(std::string(reinterpret_cast<char*>(buffer), static_cast<std::size_t>(transferred)));
}

// Send a control message
//Error UsbControl(libusb_device_handle* handle, ...)
//{
//	libusb_control_transfer(handle,
//		uint8_t request_type, uint8_t request, uint16_t value, uint16_t index,
//		unsigned char *data, uint16_t length, unsigned int timeout);
//}


SResult<device_handle> UsbOpen()
{
	libusb_device** list;
	ssize_t cnt = libusb_get_device_list(nullptr, &list);
	if (cnt < 1)
	{
		return Err(std::string("Couldn't get usb device list."));
	}

	std::unique_ptr<libusb_device*, std::function<void(libusb_device**)>>
	    autoDeleteList(list,
	                   [](libusb_device** l) {
	    libusb_free_device_list(l, 1);
    });

	int err = 0;

	libusb_device* found = nullptr;
	for (ssize_t i = 0; i < cnt; ++i)
	{
		libusb_device* device = list[i];
		libusb_device_descriptor desc;
		libusb_get_device_descriptor(device, &desc);
		
		if (desc.idVendor != VENDOR_ID_GRAPHTEC)
			continue;
		
		if (PRODUCT_ID_LIST.count(desc.idProduct) == 0)
			continue;

		libusb_device_handle* handle;

		// Currently use the first device found.
		err = libusb_open(found, &handle);
		if (err != 0)
		{
			return Err("Error accessing vinyl cutter: " + UsbError(err) +
			           ". Do you have permission (on Linux make sure you are in the group 'lp').");
		}
		device_handle devH(handle);
		return Ok(std::move(devH));
	}

	return Err(std::string("Couldn't find supported device. Is it connected to the system and powered "
	                       "on? The current list of supported devices includes the CC200, CC300, "
	                       "Silhouette SD 1, SD 2, Cameo, Cameo 3 and Portrait."));
}

// caller can use UsbSend() afterwards, and should
// finally do libusb_release_interface(handle, 0); libusb_close(handle);
SResult<device_handle> UsbInit()
{
	// Now do stuff with the handle.
	int r = 0;

	SResult<device_handle> handleRes = UsbOpen();
	if (!handleRes)
		return handleRes;

	device_handle handle = std::move(handleRes.unwrap());

	if (libusb_kernel_driver_active(handle.get(), 0) == 1)
	{
		r = libusb_detach_kernel_driver(handle.get(), 0);
		if (r != 0)
		{
			return Err("Error detaching kernel USB driver: " + UsbError(r));
		}
	}

	r = libusb_reset_device(handle.get());
	if (r != 0)
	{
		return Err("Error resetting device: " + UsbError(r));
	}

	std::cout << "Selecting configuration.\n";
	r = libusb_set_configuration(handle.get(), 1);
	if (r < 0)
	{
		return Err("Error setting USB configuration: " + UsbError(r));
	}

	std::cout << "Claiming main control interface.\n";
	r = libusb_claim_interface(handle.get(), 0);
	if (r < 0)
	{
		return Err("Error claiming USB interface: " + UsbError(r));
	}

	std::cout << "Setting alt interface.\n";
	r = libusb_set_interface_alt_setting(handle.get(), 0, 0); // Probably not really necessary.
	if (r < 0)
	{
		return Err("Error setting alternate USB interface: " + UsbError(r));
	}

	std::cout << "Initialisation successful.\n";
	return Ok(std::move(handle));
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

SResult<> Cut(CutParams p)
{
	std::cout << "Cutting... "
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

	auto handleRes = UsbInit();

	if (!handleRes)
	{
		return handleRes;
	}

	auto handle = std::move(handleRes.unwrap());
	
	// TODO: Renable this.
//	if (id.usb_vendor_id == VENDOR_ID_GRAPHTEC &&
//	        (id.usb_product_id == PRODUCT_ID_SILHOUETTE_CAMEO ||
//	         id.usb_product_id == PRODUCT_ID_SILHOUETTE_CAMEO_3))
//	{
//		// should this also transform the regwidth regheigth or not?
//		p.cuts = Transform_Silhouette_Cameo(p.cuts, &p.mediawidth, &p.mediaheight);
//	}

	SResult<> e;
	SResult<std::string> sr;
	std::string resp;

	// Initialise plotter.
	e = UsbSend(handle, "\x1b\x04");
	if (!e) return e;

	// Status request.
	e = UsbSend(handle, "\x1b\x05");
	if (!e) return e;

	sr = UsbReceive(handle, 5000);
	if (!sr) return sr;

	resp = sr.unwrap();
	if (resp != "0\x03") // 0 = Ready. 1 = Moving. 2 = Nothing loaded. "  " = ??
	{
		if (resp == "1\x03")
		  return Err(std::string("Moving, please try again."));
		if (resp == "2\x03")
		  return Err(std::string("Empty tray, please load media."));	// Silhouette Cameo
		return Err(std::string("Invalid response from plotter: ") + resp);
	}

	// Home the cutter.
	e = UsbSend(handle, "TT\x03");
	if (!e) return e;

	// Query version.
	e = UsbSend(handle, "FG\x03");
	if (!e) return e;

	// Receive the firmware version.
	sr = UsbReceive(handle, 10000); // Large timeout because the plotter moves.
	if (!sr) return sr;

	resp = sr.unwrap();
	// Don't really care about this.
//	if (resp.length() != 10)
//	{
//		e = Error("Version error: " + version);
//		goto error;
//	}

	e = UsbSend(handle, "FW" + ItoS(p.media) + "\x03");
	if (!e) return e;

	e = UsbSend(handle, "!" + ItoS(p.speed) + "\x03");
	if (!e) return e;

	e = UsbSend(handle, "FX" + ItoS(p.pressure) + "\x03");
	if (!e) return e;

	// I think this sets the distance from the position of the plotter
	// head to the actual cutting point, maybe in 0.1 mms (todo: Measure blade).
	// It is 0 for the pen, 18 for cutting.
	// C possible stands for curvature. Not that any of the other letters make sense...
	e = UsbSend(handle, "FC" + ItoS(p.media == 113 ? 0 : 18) + "\x03");
	if (!e) return e;

	e = UsbSend(handle, "FY" + ItoS(p.trackenhancing ? 0 : 1) + "\x03");
	if (!e) return e;

	// Set to portrait. FN1 does landscape but it's easier just to rotate the image.
	e = UsbSend(handle, "FN0\x03");
	if (!e) return e;

	e = UsbSend(handle, "FE0\x03"); // No idea what this does.
	if (!e) return e;

	e = UsbSend(handle, "TB71\x03"); // Again, no idea. Maybe something to do with registration marks?
	if (!e) return e;

	sr = UsbReceive(handle, 10000); // Allow 10s. Seems reasonable.
	if (!sr) return e;

	resp = sr.unwrap();

	if (resp != "    0,    0\x03")
	{
		return Err(std::string("Invalid response from plotter."));
	}


	// Begin page definition.
	e = UsbSend(handle, "FA\x03");
	if (!e) return e;


	// Block for all the "jump to error crosses initialization" errors. Really need to use exceptions!
	{
		// Page size: height,width in 20ths of a mm minus a margin. This is for A4. TODO: Find maximum and use that.
		std::stringstream page;

		int width = static_cast<int>(lround(p.mediawidth * 20.0));
		int height = static_cast<int>(lround(p.mediaheight * 20.0));

		int margintop = 0;//ProgramOptions::Instance().getMarginTop();
		int marginright = 0;//ProgramOptions::Instance().getMarginRight();

		e = UsbSend(handle, "FU" + ItoS(height - margintop) + "," + ItoS(width - marginright) + "\x03");
		if (!e) return e;
		e = UsbSend(handle, "FM1\x03"); // ?
		if (!e) return e;

		if (p.regmark)
		{
			std::stringstream regmarkstr;
			regmarkstr.precision(0);
			std::string searchregchar = "23,";
			int regw = static_cast<int>(lround(p.regwidth * 20.0));
			int regl = static_cast<int>(lround(p.regheight * 20.0));
			e = UsbSend(handle, "TB50,381\x03"); //only with registration (it was TB50,1) ???
			if (!e) return e;
			
			if (p.regsearch)
				searchregchar ="123,";
			
			regmarkstr <<  "TB99\x03TB55,1\x03TB" + searchregchar + ItoS(regw) + "," + ItoS(regl) + "\x03";
			
			std::cout << "Registration mark string: " << regmarkstr.str() << "\n";
			
			e = UsbSend(handle, regmarkstr.str()); //registration mark test /1-2: 180.0mm / 1-3: 230.0mm (origin 15mmx20mm)
			if (!e) return e;
			
			e = UsbSend(handle, "FQ5\x03"); // only with registration ???
			if (!e) return e;

			sr = UsbReceive(handle, 40000); // Allow 40s for reply...
			if (!sr) return sr;

			resp = sr.unwrap();
			if (resp != "    0,    0\x03")
			{
				std::cout << resp << "\n";
				return Err(std::string("Couldn't find registration marks."));
			}
			// Looks like if the reg marks work it gets 3 messages back (if it fails it times out because it only gets the first message)
			sr = UsbReceive(handle, 40000); // Allow 40s for reply...
			if (!sr) return sr;

			resp = sr.unwrap();
			if (resp != "    0\x03")
			{
				std::cout << resp << "\n";
				return Err(std::string("Couldn't find registration marks."));
			}

			sr = UsbReceive(handle, 40000); // Allow 40s for reply...
			if (!sr) return sr;

			resp = sr.unwrap();
			if (resp != "    1\x03")
			{
				std::cout << resp << "\n";
				return Err(std::string("Couldn't find registration marks."));
			}
		}
		else
		{
			e = UsbSend(handle, "TB50,1\x03"); // ???
			if (!e) return e;
		}

		if (!e) return e;
		// I think this is the feed command. Sometimes it is 5588 - maybe a maximum?
		e = UsbSend(handle, "FO" + ItoS(height - margintop) + "\x03");
		if (!e) return e;

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
		if (!e) return e;
	}

	// Feed the page out.
	e = UsbSend(handle, "FO0\x03");
	if (!e) return e;

	// Halt?
	e = UsbSend(handle, "H,");
	if (!e) return e;

	return Ok();
}

