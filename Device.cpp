#include "Device.h"

#include <iostream>
#include <cstdio>
#include <errno.h>

Device::Device(struct usb_device* dev)
{
	usb_handle = usb_open(dev);
	if (!usb_handle)
	{
		cerr << "Not able to open the USB device." << endl;
		return;
	}
	
	cout << "Selecting configuration." << endl;
	int r = usb_set_configuration(usb_handle, 1);
	if (r < 0)
	{
		cerr << "Error setting configuration: " << usb_strerror() << endl;
		usb_close(usb_handle);
		usb_handle = NULL;
		return;
	}
	

	cout << "Claiming main control interface." << endl;
	r = usb_claim_interface(usb_handle, 0);
	if (r < 0)
	{
		cerr << "Error claiming interface 0: " << usb_strerror() << endl;
		usb_close(usb_handle);
		usb_handle = NULL;
		return;
	}
	
	cout << "Setting alt interface." << endl;
	r = usb_set_altinterface(usb_handle, 0);
	if (r < 0)
	{
		cerr << "Error setting alt interface: " << usb_strerror() << endl;
		usb_release_interface(usb_handle, 0);
		usb_close(usb_handle);
		usb_handle = NULL;
		return;
	}
	
	cout << "Initialisation successful." << endl;
	
}

Device::~Device()
{
	if (usb_handle)
	{
		usb_release_interface(usb_handle, 0);
		usb_close(usb_handle);
	}
}

bool Device::Ok() const
{
	if (!usb_handle)
		cerr << "Warning: Not ok." << endl;
	return usb_handle;
}

void Device::Send(const string& s, int timeout)
{
	if (!Ok())
		return;
	
	for (int i = 0; i < s.length(); i += 4096) // TODO: Get 4096 from the device.
	{
		string data = s.substr(i, 4096);
		int r = usb_bulk_write(usb_handle, 1, const_cast<char*>(data.c_str()), data.length(), timeout);
		if (r < 0)
		{
			cerr << "Error writing: " << usb_strerror() << endl;
			usb_release_interface(usb_handle, 0);
			usb_close(usb_handle);
			usb_handle = NULL;
			return;
		}
		cout << "Sent: ";
		for (int i = 0; i < s.length(); ++i)
			printf ("%0.2x ", s[i]);
		cout << endl;
	}
}

string Device::Receive(int length, int timeout)
{
	if (!Ok())
		return string();
	
	char buffer[length];
	
	int r = usb_bulk_read(usb_handle, 2, buffer, length, timeout);
	if (r < 0)
	{
		if (string(usb_strerror()) == "No error")
		{
			cerr << "Nothing to read?" << endl;
			return string();
		}
		cerr << "Error reading: " << usb_strerror() << endl;
		usb_release_interface(usb_handle, 0);
		usb_close(usb_handle);
		usb_handle = NULL;
		return string();
	}
	
	cout << "Received: ";
	for (int i = 0; i < r; ++i)
		printf ("%0.2x ", buffer[i]);
	cout << endl;
	return string(buffer, r);
}
