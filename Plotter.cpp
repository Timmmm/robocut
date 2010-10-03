#include "Plotter.h"

const int VENDOR_ID = 0x0b4d;
const int PRODUCT_ID = 0x110a;

#include <iostream>

Plotter::Plotter() : dev(NULL)
{
	usb_init();

	usb_find_busses();
	usb_find_devices();

	for (usb_bus* bus = usb_busses; bus; bus = bus->next)
	{	
		for (struct usb_device* usb_dev = bus->devices; usb_dev; usb_dev = usb_dev->next)
		{
			if (usb_dev->descriptor.idVendor ==	VENDOR_ID
					&& usb_dev->descriptor.idProduct == PRODUCT_ID)
			{
				dev = new Device(usb_dev);
				return;
			}
		}
	}
}
Plotter::~Plotter()
{
	if (dev)
		delete dev;
}

void Plotter::StartPage()
{
	page.clear();
}

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
}


void Plotter::SendPage()
{
	if (!dev)
		return;
	
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
	
	dev->Send(send, 0);
//	dev->Send("FW300\x03!10\x03""FX0,0\x03""FC18\x03", 0); // Cutting mode - stop the weird noise!
}

