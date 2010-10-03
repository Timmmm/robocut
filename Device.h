#pragma once

#include "NoCopy.h"

#include <usb.h>
#include <string>

using namespace std;

class Device
{
public:
	Device(struct usb_device* dev);
	~Device();
	
	bool Ok() const;
	
	void Send(const string& s, int timeout = 1000);
	string Receive(int length, int timeout = 1000);
	
private:
	usb_dev_handle* usb_handle;
	
	NOCOPY;
};
