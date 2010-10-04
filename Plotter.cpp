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


Error UsbSend(libusb_device_handle* handle, const string& s, int timeout)
{
	for (unsigned int i = 0; i < s.length(); i += 4096) // TODO: Get 4096 from the device.
	{
		string data = s.substr(i, 4096);
		int transferred;
		int ret = libusb_bulk_transfer(handle, 1, reinterpret_cast<unsigned char*>(const_cast<char*>(data.c_str())),
									 data.length(), &transferred, timeout);
		if (ret < 0)
		{
			libusb_release_interface(handle, 0);
			libusb_close(handle);
			return Error("Error writing to device.");
		}
		if (transferred != data.length())
			cout << "Warning, some data not transferred correctly." << endl;
		cout << "Sent: ";
		for (unsigned int i = 0; i < data.length(); ++i)
			printf ("%.2x ", data[i]);
		cout << endl;
	}
	return Success;
}

string UsbReceive(libusb_device_handle* handle, int length, int timeout)
{
	unsigned char buffer[length];
	int transferred = 0;
	int ret = libusb_bulk_transfer(handle, 2, buffer, length, &transferred, timeout);
	if (ret != 0) // But it could be a timout.
	{
		libusb_release_interface(handle, 0);
		libusb_close(handle);
		return "";
	}

	cout << "Received: ";
	for (int i = 0; i < transferred; ++i)
		printf("%.2x ", buffer[i]);
	cout << endl;
	return string(reinterpret_cast<char*>(buffer), transferred);
}


Error Cut(QList<QPolygonF> cuts, int speed, int pressure)
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

	cout << "Selecting configuration." << endl;
	int r = libusb_set_configuration(handle, 1);
	if (r < 0)
	{
		libusb_close(handle);
		return Error("Error setting conf.");
	}


	cout << "Claiming main control interface." << endl;
	r = libusb_claim_interface(handle, 0);
	if (r < 0)
	{
		libusb_close(handle);
		return Error("Error claiming interface.");
	}

/*	cout << "Setting alt interface." << endl;
	r = libusb_set_altinterface(handle, 0); // Is this necessary?
	if (r < 0)
	{
		libusb_release_interface(handle, 0);
		libusb_close(handle);
		return Error("Error setting alt interface.");
	}*/

	cout << "Initialisation successful." << endl;






















/*
	dev->Send("\x1b\x04");
	dev->Send("\x1b\x05");
	cout << dev->Receive(2) << endl; // "0"
	dev->Send("TT\x03");
	dev->Send("FG\x03");
	cout << dev->Receive(10, 10000) << endl;
	dev->Send("FW113\x03"); // Pen mode.
	dev->Send("!10\x03"); // Speed = 10
	dev->Send("!10\x03FX1,0\x03"); // Speed = 10, Thickness (pressure) = 10
	dev->Send("FC0\x03"); // Curvature = 0?
	dev->Send("FY0\x03"); // 'Track enhancing' off.
	dev->Send("FE0\x03"); // ?
	dev->Send("TB71\x03"); // ?
	cout << dev->Receive(12) << endl; // "0, 0"
	dev->Send("FA\x03"); // ?
	cout << dev->Receive(12) << endl; // "0, 0"
	dev->Send("FN1\x03"); // Portrait.
	
	// Now send the page.
	string send =
"FU5440,3880\x03"
"FM1\x03"
"TB50,1\x03"
"FO5440\x03"
"&100,100,100,\0,0,Z4200,5940,L0," + page.str() + "&1,1,1,TB50,0\x03"
"FO0\x03" // This must be the feed command...?
"H,\x03";
	
	dev->Send(send, 0);*/
//	dev->Send("FW300\x03!10\x03""FX0,0\x03""FC18\x03", 0); // Cutting mode - stop the weird noise!












	libusb_release_interface(handle, 0);
	libusb_close(handle);

	return Success;
}

