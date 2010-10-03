#pragma once

#include "Device.h"
#include <string>
#include <sstream>

using namespace std;

class Plotter
{
public:
	Plotter();
	~Plotter();
	
	void StartPage();
	
	void MoveTo(double x, double y);
	void DrawTo(double x, double y);
	void SetPressurePage(int p);
	void SetSpeedPage(int p);
	
	void SendPage();
		
private:
	Device* dev;
	stringstream page;
};
