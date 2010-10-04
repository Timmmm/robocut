#include "Plotter.h"

#include <libusb-1.0/libusb.h>

const int VENDOR_ID = 0x0b4d;
const int PRODUCT_ID = 0x110a;

#include <iostream>
/*
void Plotter::MoveTo(double x, double y)
{
	if (x < 0.0)
		x = 0.0;
	if (y < 0.0)
		y = 0.0;
	if (x > 3880.0)
		x = 3880.0;
	if (y > 5440.0)
		y = 5440.0;
	page << "M" << x << "," << y << ",";
}
void Plotter::DrawTo(double x, double y)
{
	page << "D" << x << "," << y << "\x03";
	if (x < 0.0)
		x = 0.0;
	if (y < 0.0)
		y = 0.0;
	if (x > 3880.0)
		x = 3880.0;
	if (y > 5440.0)
		y = 5440.0;
}

void Plotter::SetPressurePage(int p)
{
	if (p < 1)
		p = 1;
	if (p > 33)
		p = 33;
	
	page << "FX" << p << ",0\x03";
}
void Plotter::SetSpeedPage(int p)
{
	if (p < 1)
		p = 1;
	if (p > 10)
		p = 10;
	
	page << "!" << p << "\x03";
}*/


Error UsbSend(libusb_device_handle* handle, const string& s, int timeout = 0)
{
	for (unsigned int i = 0; i < s.length(); i += 4096) // TODO: Get 4096 from the device. (Or should it be 64?)
	{
		string data = s.substr(i, 4096);
		int transferred;
		unsigned char endpoint = '\x01';
		int ret = libusb_bulk_transfer(handle, endpoint, reinterpret_cast<unsigned char*>(const_cast<char*>(data.c_str())),
									 data.length(), &transferred, timeout);
		if (ret < 0)
		{
			libusb_release_interface(handle, 0);
			libusb_close(handle);
			cerr << "Error writing to device: " << ItoS(ret) << endl;
			return Error("Error writing to device: " + ItoS(ret));
		}
		if (transferred != data.length())
			cout << "Warning, some data not transferred correctly." << endl;
		cout << "Sent: ";
		for (unsigned int i = 0; i < transferred; ++i)
			printf ("%.2x ", data[i]);
		cout << endl;
	}
	return Success;
}

string UsbReceiveLength(libusb_device_handle* handle, int length, int timeout)
{
	unsigned char buffer[length];
	int transferred = 0;
	unsigned char endpoint = '\x82';
	int ret = libusb_bulk_transfer(handle, endpoint, buffer, length, &transferred, timeout);
	if (ret != 0) // But it could be a timout.
	{
		cerr << "Error reading from device: " << ItoS(ret) << endl;
		return "";
	}

	cout << "Received: ";
	for (int i = 0; i < transferred; ++i)
		printf("%.2x ", buffer[i]);
	cout << endl;
	return string(reinterpret_cast<char*>(buffer), transferred);
}

string UsbReceive(libusb_device_handle* handle, int timeout = 0)
{
	unsigned char buffer[64]; // 1 packet size.
	int transferred = 0;
	unsigned char endpoint = '\x82';
	int ret = libusb_bulk_transfer(handle, endpoint, buffer, 64, &transferred, timeout);
	if (ret != 0) // But it could be a timout.
	{
		cerr << "Error reading from device: " << ItoS(ret) << endl;
		return "";
	}

	cout << "Received: ";
	for (int i = 0; i < transferred; ++i)
		printf("%.2x ", buffer[i]);
	cout << endl;
	return string(reinterpret_cast<char*>(buffer), transferred);
}



Error Cut(QList<QPolygonF> cuts, int media, int speed, int pressure, bool trackenhancing)
{
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
		if (desc.idVendor == VENDOR_ID && desc.idProduct == PRODUCT_ID)
		{
			found = device;
			break;
		}
	}

	if (!found)
		return Error("Couldn't connect to craft robo.");

	libusb_device_handle* handle;

	err = libusb_open(found, &handle);
	if (err)
		return Error("Error opening usb device.");

	libusb_free_device_list(list, 1);

	// Now do stuff with the handle.
	int r = 0;

	if (libusb_kernel_driver_active(handle, 0) == 1)
	{
		r = libusb_detach_kernel_driver(handle, 0);
		if (r < 0) // Maybe...
		{
			libusb_close(handle);
			return Error("Error detaching kernel driver: " + ItoS(r));
		}
	}

	libusb_reset_device(handle);

	cout << "Selecting configuration." << endl;
	r = libusb_set_configuration(handle, 1);
	if (r < 0)
	{
		libusb_close(handle);
		return Error("Error setting conf: " + ItoS(r));
	}


	cout << "Claiming main control interface." << endl;
	r = libusb_claim_interface(handle, 0);
	if (r < 0)
	{
		libusb_close(handle);
		return Error("Error claiming interface: " + ItoS(r));
	}

	cout << "Setting alt interface." << endl;
	r = libusb_set_interface_alt_setting(handle, 0, 0); // Probably not really necessary.
	if (r < 0)
	{
		return Error("Error setting alt interface: " + ItoS(r));
	}

	cout << "Initialisation successful." << endl;


	// TODO: Use exceptions.


	// Initialise plotter.
	UsbSend(handle, "\x1b\x04");

	// Status request.
	UsbSend(handle, "\x1b\x05");
	string resp = UsbReceive(handle, 5000);
	if (resp != "0\x03") // 0 = Ready. 1 = Moving. "  " = ??
	{
		libusb_release_interface(handle, 0);
		libusb_close(handle);
		return Error("Invalid response from plotter: " + resp);
	}

	// Home the cutter.
	UsbSend(handle, "TT\x03");

	// Query version.
	UsbSend(handle, "FG\x03");

	string version = UsbReceive(handle, 10000); // Large timeout because the plotter moves.
/*	if (version.length() != 10)
	{
		libusb_release_interface(handle, 0);
		libusb_close(handle);
		return Error("Version error: " + version);
	}*/

	UsbSend(handle, "FW" + ItoS(media) + "\x03");
	UsbSend(handle, "!" + ItoS(speed) + "\x03");
	UsbSend(handle, "FX" + ItoS(pressure) + "\x03");

	// I think this sets the distance from the position of the plotter
	// head to the actual cutting point, maybe in 0.1 mms (todo: Measure blade).
	// It is 0 for the pen, 18 for cutting.
	// C possible stands for curvature. Not that any of the other letters make sense...
	UsbSend(handle, "FC" + ItoS(media == 113 ? 0 : 18) + "\x03");

	UsbSend(handle, "FY" + ItoS(trackenhancing ? 1 : 0) + "\x03");

	// Set to portrait. FN1 does landscape but it's easier just to rotate the image.
	UsbSend(handle, "FN0\x03");

	UsbSend(handle, "FE0\x03"); // No idea what this does.

	UsbSend(handle, "TB71\x03"); // Again, no idea. Maybe something to do with registration marks?

	if (UsbReceive(handle, 5000) != "    0,    0\x03")
	{
		libusb_release_interface(handle, 0);
		libusb_close(handle);
		return Error("Invalid response from plotter.");
	}


	// Begin page definition.
	UsbSend(handle, "FA\x03");

	// Page size: height,width in 20ths of a mm minus a margin. This is for A4. TODO: Find maximum and use that.
	// Could be 16ths of a mm plus some margin?

	// Maybe this actually sets the maximum values. I don't know.

	int width = 20 * 210;
	int height = 20 * 297;

	int margintop = 500;
	int marginright = 320;

	UsbSend(handle, "FU" + ItoS(height - margintop) + "," + ItoS(width - marginright) + "\x03");
	UsbSend(handle, "FM1\x03"); // ?
	UsbSend(handle, "TB50,1\x03"); // ?

	// I think this is the feed command. Sometimes it is 5588 - maybe a maximum?
	UsbSend(handle, "FO" + ItoS(height - margintop) + "\x03");

	stringstream page;
	page.flags(ios::fixed);
	page.precision(3);
	page << "&100,100,100,\\0,0,Z" << ItoS(width) << "," << ItoS(height) << ",L0";

	for (int i = 0; i < cuts.size(); ++i)
	{
		if (cuts[i].size() < 2)
			continue;
		page << ",M" << cuts[i][0].x()*20.0 << "," << height-cuts[i][0].y()*20.0;
		for (int j = 1; j < cuts[i].size(); ++j)
		{
			page << ",D" << cuts[i][j].x()*20.0 << "," << height-cuts[i][j].y()*20.0;
		}
	}

	page << "&1,1,1,TB50,0\x03"; // TB maybe .. ah I dunno. Need to experiment. No idea what &1,1,1 does either.

	cout << page.str() << endl;
	UsbSend(handle, page.str());

	// Feed the page out.
	UsbSend(handle, "FO0\x03");

	// Halt?
	UsbSend(handle, "H,");


	libusb_release_interface(handle, 0);
	libusb_close(handle);

	return Success;
}

