#include "Plotter.h"

#include <libusb-1.0/libusb.h>

const int VENDOR_ID = 0x0b4d;
const int PRODUCT_ID = 0x110a;

#include <iostream>


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
		if (transferred != data.length())
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



Error Cut(QList<QPolygonF> cuts, int media, int speed, int pressure, bool trackenhancing)
{
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

	libusb_device** list;
	ssize_t cnt = libusb_get_device_list(NULL, &list);
	if (cnt < 1)
		return Error("Couldn't get usb device list.");

	int err = 0;

	libusb_device* found = NULL;
	for (ssize_t i = 0; i < cnt; ++i)
	{
		libusb_device* device = list[i];
		libusb_device_descriptor desc;
		libusb_get_device_descriptor(device, &desc);
		// I don't want to be more specific than this really.
		if (desc.idVendor == VENDOR_ID && desc.idProduct == PRODUCT_ID)
		{
			// Just use the first one. Who has two?!
			found = device;
			break;
		}
	}

	if (!found)
	{
		libusb_free_device_list(list, 1);
		return Error("Couldn't find Craft Robo 2 (USB device with Vendor ID with USB VendorID 0b4d and ProductID 110a). Is it connected to the system and powered on?");
	}

	libusb_device_handle* handle;

	err = libusb_open(found, &handle);
	if (err != 0)
	{
		libusb_free_device_list(list, 1);
		return Error("Error accessing Craft Robo 2: " + UsbError(err) + ". Do you have permission (on Linux make sure you are in the group 'lp').");
	}

	libusb_free_device_list(list, 1);

	// Now do stuff with the handle.
	int r = 0;

	if (libusb_kernel_driver_active(handle, 0) == 1)
	{
		r = libusb_detach_kernel_driver(handle, 0);
		if (r != 0)
		{
			libusb_close(handle);
			return Error("Error detaching kernel USB driver: " + UsbError(r));
		}
	}

	r = libusb_reset_device(handle);
	if (r != 0)
	{
		libusb_close(handle);
		return Error("Error resetting device: " + UsbError(r));
	}

	cout << "Selecting configuration." << endl;
	r = libusb_set_configuration(handle, 1);
	if (r < 0)
	{
		libusb_close(handle);
		return Error("Error setting USB configuration: " + UsbError(r));
	}


	cout << "Claiming main control interface." << endl;
	r = libusb_claim_interface(handle, 0);
	if (r < 0)
	{
		libusb_close(handle);
		return Error("Error claiming USB interface: " + UsbError(r));
	}

	cout << "Setting alt interface." << endl;
	r = libusb_set_interface_alt_setting(handle, 0, 0); // Probably not really necessary.
	if (r < 0)
	{
		return Error("Error setting alternate USB interface: " + UsbError(r));
	}

	cout << "Initialisation successful." << endl;


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

	if (resp != "0\x03") // 0 = Ready. 1 = Moving. "  " = ??
	{
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

	e = UsbSend(handle, "FY" + ItoS(trackenhancing ? 1 : 0) + "\x03");
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

		int width = 20 * 210;
		int height = 20 * 297;

		int margintop = 500;
		int marginright = 320;

		e = UsbSend(handle, "FU" + ItoS(height - margintop) + "," + ItoS(width - marginright) + "\x03");
		if (!e) goto error;
		e = UsbSend(handle, "FM1\x03"); // ?
		if (!e) goto error;
		e = UsbSend(handle, "TB50,1\x03"); // ?
		if (!e) goto error;

		// I think this is the feed command. Sometimes it is 5588 - maybe a maximum?
		e = UsbSend(handle, "FO" + ItoS(height - margintop) + "\x03");
		if (!e) goto error;

		page.flags(ios::fixed);
		page.precision(3);
		page << "&100,100,100,\\0,0,Z" << ItoS(width) << "," << ItoS(height) << ",L0";

		for (int i = 0; i < cuts.size(); ++i)
		{
			if (cuts[i].size() < 2)
				continue;

			double x = cuts[i][0].x()*20.0;
			double y = cuts[i][0].y()*20.0;

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

				if (x < 0.0) x = 0.0;
				if (x > width) x = width;

				if (y < 0.0) y = 0.0;
				if (y > height) y = height;
				page << ",D" << x << "," << height-y;
			}
		}

		page << "&1,1,1,TB50,0\x03"; // TB maybe .. ah I dunno. Need to experiment. No idea what &1,1,1 does either.

		cout << page.str() << endl;

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
	libusb_release_interface(handle, 0);
	libusb_close(handle);
	return e;

}

