/***************************************************************************
 *   This file is part of Robocut.                                         *
 *   Copyright (C) 2010 Tim Hutt <tdhutt@gmail.com>                        *
 *   Copyright (C) 2010 Markus Schulz <schulz@alpharesearch.de>            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.              *
 ***************************************************************************/

#include "Plotter.h"
#include "ProgramOptions.h"

#include <libusb-1.0/libusb.h>

      int VENDOR_ID = ProgramOptions::Instance().getVendorUSB_ID();
      int PRODUCT_ID = ProgramOptions::Instance().getProductUSB_ID();

#include <iostream>
#include <cmath>


string UsbError(int e)
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
	// Send in 4096 byte chunks. Not sure where I got this from, I'm not sure it is actually necessary.
	for (unsigned int i = 0; i < s.length(); i += 4096)
	{
		string data = s.substr(i, 4096);
		int transferred;
		unsigned char endpoint = '\x01';
		int ret = libusb_bulk_transfer(handle, endpoint, reinterpret_cast<unsigned char*>(const_cast<char*>(data.c_str())),
									 data.length(), &transferred, timeout);
		if (ret != 0)
		{
			cerr << "Error writing to device: " << UsbError(ret) << endl;
			return Error("Error writing to device: " + UsbError(ret));
		}
		if ((unsigned int)transferred != data.length())
		{
			cerr << "Warning, some data not transferred correctly." << endl;
			return Error("Some data not transfered. Attempted: " + ItoS(data.length()) + " Transferred: " + ItoS(transferred));
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
	unsigned char endpoint = '\x82';
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

libusb_device_handle *UsbOpen(struct cutter_id *id)
{
	libusb_device** list;
	ssize_t cnt = libusb_get_device_list(NULL, &list);
	if (cnt < 1)
	{
		id->msg = Error("Couldn't get usb device list.");
		return NULL;
	}

	int err = 0;

	libusb_device* found = NULL;
	for (ssize_t i = 0; i < cnt; ++i)
	{
		libusb_device* device = list[i];
		libusb_device_descriptor desc;
		libusb_get_device_descriptor(device, &desc);
		// I don't want to be more specific than this really.
		if ((desc.idVendor == VENDOR_ID || desc.idVendor == VENDOR_ID_GRAPHTEC) && 
		    (desc.idProduct == PRODUCT_ID ||
		     desc.idProduct == PRODUCT_ID_CC200_20 ||
		     desc.idProduct == PRODUCT_ID_CC300_20 ||
		     desc.idProduct == PRODUCT_ID_SILHOUETTE_SD_1 ||
		     desc.idProduct == PRODUCT_ID_SILHOUETTE_SD_2 ||
		     desc.idProduct == PRODUCT_ID_SILHOUETTE_CAMEO ||
		     desc.idProduct == PRODUCT_ID_SILHOUETTE_PORTRAIT
		     )
		    )
		{
			// Just use the first one. Who has two?!
			found = device;
			id->usb_product_id = desc.idProduct;
			id->usb_vendor_id = desc.idVendor;
			break;
		}
	}

	if (!found)
	{
		libusb_free_device_list(list, 1);
		id->msg = Error("Couldn't find Craft Robo 2 (USB device with Vendor ID with USB VendorID 0b4d and ProductID 110a). Is it connected to the system and powered on?");
		return NULL;
	}

	libusb_device_handle* handle;

	err = libusb_open(found, &handle);
	if (err != 0)
	{
		libusb_free_device_list(list, 1);
		id->msg = Error("Error accessing Craft Robo 2: " + UsbError(err) + ". Do you have permission (on Linux make sure you are in the group 'lp').");
		return NULL;
	}

	libusb_free_device_list(list, 1);
	id->msg = "Ready";
	return handle;
}

// caller can use UsbSend() afterwards, and should
// finally do libusb_release_interface(handle, 0); libusb_close(handle);
libusb_device_handle *UsbInit(struct cutter_id *id)
{
	// Now do stuff with the handle.
	int r = 0;

	libusb_device_handle* handle = UsbOpen(id);
	if (!handle) return NULL;

	if (libusb_kernel_driver_active(handle, 0) == 1)
	{
		r = libusb_detach_kernel_driver(handle, 0);
		if (r != 0)
		{
			libusb_close(handle);
			id->msg = Error("Error detaching kernel USB driver: " + UsbError(r));
			return NULL;
		}
	}

	r = libusb_reset_device(handle);
	if (r != 0)
	{
		libusb_close(handle);
		id->msg = Error("Error resetting device: " + UsbError(r));
		return NULL;
	}

	cout << "Selecting configuration." << endl;
	r = libusb_set_configuration(handle, 1);
	if (r < 0)
	{
		libusb_close(handle);
		id->msg = Error("Error setting USB configuration: " + UsbError(r));
		return NULL;
	}


	cout << "Claiming main control interface." << endl;
	r = libusb_claim_interface(handle, 0);
	if (r < 0)
	{
		libusb_close(handle);
		id->msg = Error("Error claiming USB interface: " + UsbError(r));
		return NULL;
	}

	cout << "Setting alt interface." << endl;
	r = libusb_set_interface_alt_setting(handle, 0, 0); // Probably not really necessary.
	if (r < 0)
	{
		libusb_close(handle);
		id->msg = Error("Error setting alternate USB interface: " + UsbError(r));
		return NULL;
	}

	cout << "Initialisation successful." << endl;
	return handle;
}

struct cutter_id *Identify()
{
	static struct cutter_id id = { "?", 0, 0 };
	libusb_device_handle *handle;

	if (1)
	  {
	    handle = UsbOpen(&id);
	    if (handle)
	      libusb_close(handle);
	    else
	      id.msg = "no device found";
	  }
	else
	  {
	    id.msg = Error("Cannot Identify while cut thread is running");
	  }
	return &id;
}


QList<QPolygonF> Transform_Silhouette_Cameo(QList<QPolygonF> cuts, double *mediawidth, double *mediaheight)
{
	const double devicewidth = 300;		// cameo is 12inch aka 300mm wide
	double w;
	double h = *mediaheight;
	
	cout << "Transform_Silhouette_Cameo " << *mediawidth << "," << *mediaheight << endl;
	if (0) 
	{
		w = *mediawidth;
		cout << "Paper right aligned" << endl;
	}
	else
	{
		w = devicewidth;
		// adjust the used media area, so that the hardware does not clip.
		*mediawidth = devicewidth;
		cout << "Paper left aligned" << endl;
	}
	  

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

Error Cut(QList<QPolygonF> cuts, double mediawidth, double mediaheight, int media, int speed, int pressure, bool trackenhancing,
		  bool regmark, bool regsearch, double regwidth, double reglength)
{
	VENDOR_ID = ProgramOptions::Instance().getVendorUSB_ID();
	PRODUCT_ID = ProgramOptions::Instance().getProductUSB_ID();
	
	cout << "Cutting... VENDOR_ID : " << VENDOR_ID << " PRODUCT_ID: " << PRODUCT_ID
		 << " mediawidth: " << mediawidth << " mediaheight: " << mediaheight
		 << "media: " << media << " speed: " << speed << " pressure: " << pressure
		 << " trackenhancing: " << trackenhancing << " regmark: " <<  regmark
		 << " regsearch:" <<  regsearch <<" regwidth:" <<  regwidth << " reglength: " << reglength << endl;

	if (media < 100 || media > 300)
		media = 300;
	if (speed < 1)
		speed = 1;
	if (speed > 10)
		speed = 10;
	if (pressure < 1)
		pressure = 1;
	if (pressure > 33)
		pressure = 33;

	// how can VENDOR_ID / PRODUCT_ID report the correct values abve???
	struct cutter_id id = { "?", 0, 0 };
	libusb_device_handle* handle = UsbInit(&id);
	if (id.usb_vendor_id == VENDOR_ID_GRAPHTEC &&
	   id.usb_product_id == PRODUCT_ID_SILHOUETTE_CAMEO)
	  {
	    // should this also transform the regwidth regheigth or not?
	    cuts = Transform_Silhouette_Cameo(cuts, &mediawidth, &mediaheight);
	  }

	// TODO: Use exceptions.

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

	e = UsbSend(handle, "FW" + ItoS(media) + "\x03");
	if (!e) goto error;

	e = UsbSend(handle, "!" + ItoS(speed) + "\x03");
	if (!e) goto error;

	e = UsbSend(handle, "FX" + ItoS(pressure) + "\x03");
	if (!e) goto error;

	// I think this sets the distance from the position of the plotter
	// head to the actual cutting point, maybe in 0.1 mms (todo: Measure blade).
	// It is 0 for the pen, 18 for cutting.
	// C possible stands for curvature. Not that any of the other letters make sense...
	e = UsbSend(handle, "FC" + ItoS(media == 113 ? 0 : 18) + "\x03");
	if (!e) goto error;

	e = UsbSend(handle, "FY" + ItoS(trackenhancing ? 0 : 1) + "\x03");
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
		stringstream page;

		int width = lroundl(mediawidth * 20.0);
		int height = lroundl(mediaheight * 20.0);

		int margintop = ProgramOptions::Instance().getMarginTop();
		int marginright = ProgramOptions::Instance().getMarginRight();

		e = UsbSend(handle, "FU" + ItoS(height - margintop) + "," + ItoS(width - marginright) + "\x03");
		if (!e) goto error;
		e = UsbSend(handle, "FM1\x03"); // ?
		if (!e) goto error;

		if (regmark)
		{
			stringstream regmarkstr;
			regmarkstr.precision(0);
			string searchregchar = "23,";
			int regw = lroundl(regwidth * 20.0);
			int regl = lroundl(reglength * 20.0);
			e = UsbSend(handle, "TB50,381\x03"); //only with registration (it was TB50,1) ???
			if (!e) goto error;
			
			if (regsearch)
				searchregchar ="123,";
			
			regmarkstr <<  "TB99\x03TB55,1\x03TB" + searchregchar + ItoS(regw) + "," + ItoS(regl) + "\x03";
			
			cout << "Registration mark string: " << regmarkstr.str() << endl;
			
			e = UsbSend(handle, regmarkstr.str()); //registration mark test /1-2: 180.0mm / 1-3: 230.0mm (origin 15mmx20mm)
			if (!e) goto error;
			
			e = UsbSend(handle, "FQ5\x03"); // only with registration ???
			if (!e) goto error; 

			e = UsbReceive(handle, resp, 40000); // Allow 20s for reply...
						if (!e) goto error;
			if (resp != "    0,    0\x03")
			{
				cout << resp << endl;
				e = Error("Couldn't find registration marks.");
				goto error;
			}
// Looks like if the reg marks work it gets 3 messages back (if it fails it times out because it only gets the first message)
			e = UsbReceive(handle, resp, 40000); // Allow 20s for reply...
			if (!e) goto error;
			if (resp != "    0\x03")
			{
				cout << resp << endl;
				e = Error("Couldn't find registration marks.");
				goto error;
			}

			e = UsbReceive(handle, resp, 40000); // Allow 20s for reply...
			if (!e) goto error;
			if (resp != "    1\x03")
			{
				cout << resp << endl;
				e = Error("Couldn't find registration marks.");
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

		page.flags(ios::fixed);
		page.precision(0);
		page << "&100,100,100,\\0,0,Z" << ItoS(width) << "," << ItoS(height) << ",L0";
		for (int i = 0; i < cuts.size(); ++i)
		{
			if (cuts[i].size() < 2)
				continue;

			double x = cuts[i][0].x()*20.0;
			double y = cuts[i][0].y()*20.0;

			double xorigin = ProgramOptions::Instance().getRegOriginWidthMM();
			double yorigin = ProgramOptions::Instance().getRegOriginHeightMM();

			if (regmark)
			{
				x = x - (xorigin*20.0);
				y = y + (yorigin*20.0);
			}

			// TODO: Also do this in the UI and warn the user about it.
			if (x < 0.0) x = 0.0;
			if (x > width) x = width;

			if (y < 0.0) y = 0.0;
			if (y > height) y = height;

			page << ",M" << x << "," << height-y;
			for (int j = 1; j < cuts[i].size(); ++j)
			{
				x = cuts[i][j].x()*20.0;
				y = cuts[i][j].y()*20.0;

				if (regmark)
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

		// cout << page.str() << endl;

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

