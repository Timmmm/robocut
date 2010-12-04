/***************************************************************************
 *   This file is part of Robocut.                                         *
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

#include "ProgramOptions.h"
#include <cmath>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

ProgramOptions::ProgramOptions ( ) 
{
initAttributes();
}
ProgramOptions::~ProgramOptions ( ) { }
ProgramOptions& ProgramOptions::Instance()
{
	static ProgramOptions instance;
	return instance;
}


void ProgramOptions::setSortPath ( int new_var ) 
{
	sortPath = new_var;
}

int ProgramOptions::getSortPath ( ) 
{
	return sortPath;
}

void ProgramOptions::setStartCut ( int new_var ) 
{
	startCut = new_var;
}

int ProgramOptions::getStartCut ( ) 
{
	return startCut;
}

void ProgramOptions::setFileName ( QString new_var ) 
{
	fileName = new_var;
}

QString ProgramOptions::getFileName ( ) 
{
	return fileName;
}

void ProgramOptions::setTspSortPath ( int new_var ) 
{
	tspSortPath = new_var;
}

int ProgramOptions::getTspSortPath ( ) 
{
	return tspSortPath;
}

void ProgramOptions::setMedia ( int new_var ) 
{
	if (new_var<0) new_var = 0;
	if (new_var>25) new_var = 25;
	media = new_var;
}

int ProgramOptions::getMedia ( ) 
{
	return media;
}

void ProgramOptions::setSpeed ( int new_var ) 
{
	speed = new_var;
}

int ProgramOptions::getSpeed ( ) 
{
	return speed;
}

void ProgramOptions::setPressure ( int new_var ) 
{
	pressure = new_var;
}

int ProgramOptions::getPressure ( ) 
{
	return pressure;
}

void ProgramOptions::setRegMarkAuto ( int new_var ) 
{
	regMarkAuto = new_var;
}

int ProgramOptions::getRegMarkAuto ( ) 
{
	return regMarkAuto;
}

void ProgramOptions::setRegMark ( int new_var ) 
{
	regMark = new_var;
}

int ProgramOptions::getRegMark ( ) 
{
	return regMark;
}

void ProgramOptions::setRegDimensionWidth ( int new_var ) 
{
	regDimensionWidth = new_var;
}

int ProgramOptions::getRegDimensionWidth ( ) 
{
	return lroundl(regDimensionWidth);
}

void ProgramOptions::setRegDimensionHeight ( int new_var ) 
{
	regDimensionHeight = new_var;
}

int ProgramOptions::getRegDimensionHeight ( ) 
{
	return lroundl(regDimensionHeight);
}

void ProgramOptions::setRegOriginWidth ( int new_var ) 
{
	regOriginWidth = new_var;
}

int ProgramOptions::getRegOriginWidth ( ) 
{
	return lroundl(regOriginWidth);
}

void ProgramOptions::setRegOriginHeight ( int new_var ) 
{
	regOriginHeight = new_var;
}

int ProgramOptions::getRegOriginHeight ( ) 
{
	return lroundl(regOriginHeight);
}

void ProgramOptions::setRegDimensionWidthMM ( double new_var ) 
{
	regDimensionWidth = new_var*20;
}

double ProgramOptions::getRegDimensionWidthMM ( ) 
{
	return regDimensionWidth/20;
}

void ProgramOptions::setRegDimensionHeightMM ( double new_var ) 
{
	regDimensionHeight = new_var*20;
}

double ProgramOptions::getRegDimensionHeightMM ( ) 
{
	return regDimensionHeight/20;
}

void ProgramOptions::setRegOriginWidthMM ( double new_var ) 
{
	regOriginWidth = new_var*20;
}

double ProgramOptions::getRegOriginWidthMM ( ) 
{
	return regOriginWidth/20;
}

void ProgramOptions::setRegOriginHeightMM ( double new_var ) 
{
	regOriginHeight = new_var*20;
}

double ProgramOptions::getRegOriginHeightMM ( ) 
{
	return regOriginHeight/20;
}

void ProgramOptions::setVersion ( QString new_var ) 
{
	version = new_var;
}

QString ProgramOptions::getVersion ( ) 
{
	return version;
}

void ProgramOptions::setVendorUSB_ID ( int new_var ) 
{
	vendorUSB_ID = new_var;
}

int ProgramOptions::getVendorUSB_ID ( ) 
{
	return vendorUSB_ID;
}

void ProgramOptions::setProductUSB_ID ( int new_var ) 
{
	productUSB_ID = new_var;
}

int ProgramOptions::getProductUSB_ID ( ) 
{
	return productUSB_ID;
}

void ProgramOptions::setMarginTop ( int new_var ) 
{
	marginTop = new_var;
}
int ProgramOptions::getMarginTop ( ) 
{
	return lroundl(marginTop);
}
void ProgramOptions::setMarginRight ( int new_var ) 
{
	marginRight = new_var;
}
int ProgramOptions::getMarginRight ( ) 
{
	return lroundl(marginRight);
}

void ProgramOptions::setMarginTopMM ( double new_var ) 
{
	marginTop = new_var*20;
}
double ProgramOptions::getMarginTopMM ( ) 
{
	return marginTop/20;
}
void ProgramOptions::setMarginRightMM ( double new_var ) 
{
	marginRight = new_var*20;
}
double ProgramOptions::getMarginRightMM ( ) 
{
	return marginRight/20;
}

void ProgramOptions::setTrackEnhancing ( int new_var ) 
{
	trackEnhancing = new_var;
}

int ProgramOptions::getTrackEnhancing ( ) 
{
	return trackEnhancing;
} 

int ProgramOptions::GetOpt (int argc, char *argv[] )
{
	static int version_flag = 0, help_flag = 0, show_flag = 0;
	int index, option_index = 0;
	int c = 0;
	const char shortopts[] = "stg:p:ra:b:c:d:";
	static struct option longopts[] =
	{
		{"help",                 no_argument,       &help_flag, 1},
		{"version",              no_argument,       &version_flag, 1},
		{"show",                 no_argument,       &show_flag, 1},
		{"no-sort",              no_argument,       0, 's'},
		{"bb-sort",              no_argument,       0, 't'},
		{"cut",                  no_argument,       0, 'l'},
		{"media",                required_argument, 0, 'm'},
		{"speed",                required_argument, 0, 'g'},
		{"pressure",             required_argument, 0, 'p'},
		{"track-enhancing",      no_argument,       0, 'n'},
		{"margin-top",           required_argument, 0, 'o'},
		{"margin-right",         required_argument, 0, 'q'},
		{"reg-mark-auto",        no_argument,       0, 'r'},
		{"reg-mark",             no_argument,       0, 'h'},
		{"reg-dimension-width",  required_argument, 0, 'a'},
		{"reg-dimension-height", required_argument, 0, 'b'},
		{"reg-origin-width",     required_argument, 0, 'c'},
		{"reg-origin-height",    required_argument, 0, 'd'},
		{"usb-vendor-id",        required_argument, 0, 'e'},
		{"usb-product-id",       required_argument, 0, 'f'},
		{0, 0, 0, 0}
	};
	
	opterr = 0;
		while ((c = getopt_long(argc, argv, shortopts, longopts, &option_index)) != -1)
		{
			switch (c)
			{
			case 0:
			/* If this option set a flag, do nothing else now. */
				if (longopts[option_index].flag != 0) break;
				//cout << longopts[option_index].name << endl;
				break;
			case 's':
				setSortPath(true);
				break;
			case 't':
				setTspSortPath(true);
				break;
			case 'l':
				setStartCut(true);
				break;
			case 'm':
				setMedia(atoi(optarg));
				break;
			case 'g':
				setSpeed(atoi(optarg));
				break;
			case 'p':
				setPressure(atoi(optarg));
				break;
			case 'n':
				setTrackEnhancing(true);
				break;
			case 'o':
				setMarginTopMM(atof(optarg));
				break;
			case 'q':
				setMarginRightMM(atof(optarg));
				break;
			case 'r':
				setRegMarkAuto(true);
				break;
			case 'h':
				setRegMark(true);
				break;
			case 'a':
				setRegDimensionWidthMM(atof(optarg));
				break;
			case 'b':
				setRegDimensionHeightMM(atof(optarg));
				break;
			case 'c':
				setRegOriginWidthMM(atof(optarg));
				break;
			case 'd':
				setRegOriginHeightMM(atof(optarg));
				break;
			case 'e':
				setVendorUSB_ID(atoi(optarg));
				break;
			case 'f':
				setProductUSB_ID(atoi(optarg));
				break;
			case '?':
			 if (optopt == 'f')
				cerr << "Option -"<< optopt <<" requires an argument." << endl;
			else if (isprint (optopt))
				cerr << "Unknown option `-"<< (char)optopt <<"'." << endl;
			else
				cerr << "Unknown option character ASCII code "<< optopt <<"." << endl;
				exit(0);
			default:
				abort();
			}
		}
		for (index = optind; index < argc; index++)
		{
			//cout << "Non-option argument " << argv[index] <<"'." << endl;
			fileName = argv[optind]; // take the first non arg as file
			continue;
		}
		if(show_flag)
		{
			showShow();
			exit(0);
		}
		if(help_flag)
		{
			showHelp();
			exit(0);
		}
		if(version_flag)
		{
			showVersion();
			exit(0);
		}
	return 0;
}

void ProgramOptions::showHelp ( )
{
	//      "<------------------------------------------------------------------------------>"
	cout << "The `Robocut' program plots or cuts SVG files on a Craft Robo or Silhouette SD" << endl;
	cout << "or most likely any other customer grade Graphtec cutting plotter." << endl << endl;
	cout << "Usage:" << endl;
	cout << "  robocut [OPTIONS...] [File]" << endl << endl;
	//      "<------------------------------------------------------------------------------>"
	cout << "Help Options:" << endl;
	cout << "  --help                     Show summary of options." << endl;
	cout << "  --version                  Show version information and copyright details." << endl;
	cout << "  --show                     Show all the parameters entered and the defaults." << endl << endl;
	//      "<------------------------------------------------------------------------------>"
	cout << "Application Options:" << endl;
	cout << "  -s, --no-sort              Stop sort the objects from the SVG before plotting." << endl;
	cout << "  -t, --bb-sort              Sort the objects by bounding box, good for letters." << endl;
	cout << "                             This will cut out the inside first and than the" << endl;
	cout << "                             outside." << endl;
	cout << "  --cut                      Shows the cutting dialogue after start." << endl;
	cout << "  --media INT                Select the media. See drop down box." << endl;
	cout << "  -g, --speed INT            The speed between 1 and 33." << endl;
	cout << "  -p, --pressure INT         The pressure between 1 and 10." << endl;
	cout << "  --track-enhancing          Move three times back and forward to create a" << endl;
	cout << "                             track." << endl;
	cout << "  --margin-top DOUBLE        Define the margin on top in mm." << endl;
	cout << "  --margin-right DOUBLE      Define the margin right in mm." << endl << endl;
	//      "<------------------------------------------------------------------------------>"
	cout << "Registration Mark Options:" << endl;
	cout << "  -r, --reg-mark-auto        Use registration marks and find the marks" << endl;
	cout << "                             automatically." << endl;
	cout << "  --reg-mark                 Use registration marks but set the knife manually" << endl;
	cout << "                             on top the round mark." << endl;
	cout << "  -a, --reg-dimension-width DOUBLE" << endl;
	cout << "                             Specify the length in mm between the the two" << endl;
	cout << "                             horizontal marks." << endl;
	cout << "  -b, --reg-dimension-height DOUBLE" << endl;
	cout << "                             Specify the length in mm between the the two" << endl;
	cout << "                             vertical marks." << endl;
	cout << "  -c, --reg-origin-width DOUBLE" << endl;
	cout << "                             Specify the length in mm between the the edge of" << endl;
	cout << "                             the paper and the first horizontal mark." << endl;
	cout << "  -d, --reg-origin-height DOUBLE" << endl;
	cout << "                             Specify the length in mm between the the edge of" << endl;
	cout << "                             the paper and the first vertical mark." << endl << endl;
	//      "<------------------------------------------------------------------------------>"
	cout << "Device Options:" << endl;
	cout << "  --usb-vendor-id INT        Try this program with other usb hardware on" << endl;
	cout << "                             your own risk. You need to convert hex to dec." << endl;
	cout << "  --usb-product-id INT       May also work with other hardware, try" << endl;
	cout << "                             usb-devices program to scan your computer." << endl;
	cout << "Report bugs to https://bugs.launchpad.net/robocut/+filebug." << endl;
	//      "<------------------------------------------------------------------------------>"
}


void ProgramOptions::showVersion ( )
{
	cout << (version.toStdString()); // see main.cpp
	cout << "\n\nCopyright (C) 2010\n";
	cout << "This is free software; see the source for copying conditions.  There is NO\n";
	cout << "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\n";
	cout << "Written by Tim Hutt and Markus Schulz <schulz@alpharesearch.de>\n";
}

void ProgramOptions::showShow ( )
{
	cout << "Application Options:\n";
	cout << "  -s, --no-sort              "<< getSortPath() << endl;
	cout << "  -t, --bb-sort              "<< getTspSortPath() << endl;
	cout << "  --cut                      "<< getStartCut() << endl;
	cout << "  --media                    "<< getMedia() << endl;
	cout << "  -g, --speed                "<< getSpeed() << endl;
	cout << "  -p, --pressure             "<< getPressure() << endl;
	cout << "  --track-enhancing          "<< getTrackEnhancing() << endl;
	cout << "  --margin-top               "<< getMarginTopMM() << " / " << getMarginTop() << endl;
	cout << "  --margin-right             "<< getMarginRightMM() << " / " << getMarginRight() << endl;
	cout << "  -r, --reg-mark-auto        "<< getRegMarkAuto() << endl;
	cout << "  --reg-mark                 "<< getRegMark() << endl;
	cout << "  -a, --reg-dimension-width  "<< getRegDimensionWidthMM() << " / " << getRegDimensionWidth() << endl;
	cout << "  -b, --reg-dimension-height "<< getRegDimensionHeightMM() << " / " << getRegDimensionHeight() << endl;
	cout << "  -c, --reg-origin-width     "<< getRegOriginWidthMM() << " / " << getRegOriginWidth() << endl;
	cout << "  -d, --reg-origin-height    "<< getRegOriginHeightMM() << " / " << getRegOriginHeight() << endl;
	cout << "  --usb-vendor-id            "<< getVendorUSB_ID() << endl;
	cout << "  --usb-product-id           "<< getProductUSB_ID() << endl;
	cout << "[File]                       "<< getFileName().toStdString() << endl;
}

void ProgramOptions::initAttributes ( ) {
	sortPath = false;
	tspSortPath = false; // used for BBox
	startCut = false;
	fileName = "";
	media = 2;
	speed = 10;
	pressure = 10;
	regMarkAuto = false;
	regMark = false;
	setRegDimensionWidthMM(180.0);
	setRegDimensionHeightMM(240.0);
	setRegOriginWidthMM(15.0);
	setRegOriginHeightMM(10.0);
	vendorUSB_ID = 0x0b4d;
	productUSB_ID = 0x111d;
	setMarginTop(500);
	setMarginRight(320);
	trackEnhancing = false;
}

