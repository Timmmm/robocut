#include "ProgramOptions.h"
#include <iostream>
using namespace std;

// Constructors/Destructors
//  

ProgramOptions::ProgramOptions ( ) {
initAttributes();
}

ProgramOptions::~ProgramOptions ( ) { }

//  
// Methods
//  


// Accessor methods
//  


// Private attribute accessor methods
//  


/**
 * Set the value of sortPath
 * @param new_var the new value of sortPath
 */
void ProgramOptions::setSortPath ( bool new_var ) {
	sortPath = new_var;
}

/**
 * Get the value of sortPath
 * @return the value of sortPath
 */
bool ProgramOptions::getSortPath ( ) {
	return sortPath;
}

/**
 * Set the value of startCut
 * @param new_var the new value of startCut
 */
void ProgramOptions::setStartCut ( bool new_var ) {
	startCut = new_var;
}

/**
 * Get the value of startCut
 * @return the value of startCut
 */
bool ProgramOptions::getStartCut ( ) {
	return startCut;
}

/**
 * Set the value of fileName
 * @param new_var the new value of fileName
 */
void ProgramOptions::setFileName ( QString new_var ) {
	fileName = new_var;
}

/**
 * Get the value of fileName
 * @return the value of fileName
 */
QString ProgramOptions::getFileName ( ) {
	return fileName;
}

/**
 * Set the value of tspSortPath
 * @param new_var the new value of tspSortPath
 */
void ProgramOptions::setTspSortPath ( bool new_var ) {
	tspSortPath = new_var;
}

/**
 * Get the value of tspSortPath
 * @return the value of tspSortPath
 */
bool ProgramOptions::getTspSortPath ( ) {
	return tspSortPath;
}

/**
 * Set the value of media
 * @param new_var the new value of media
 */
void ProgramOptions::setMedia ( int new_var ) {
	media = new_var;
}

/**
 * Get the value of media
 * @return the value of media
 */
int ProgramOptions::getMedia ( ) {
	return media;
}

/**
 * Set the value of speed
 * @param new_var the new value of speed
 */
void ProgramOptions::setSpeed ( int new_var ) {
	speed = new_var;
}

/**
 * Get the value of speed
 * @return the value of speed
 */
int ProgramOptions::getSpeed ( ) {
	return speed;
}

/**
 * Set the value of pressure
 * @param new_var the new value of pressure
 */
void ProgramOptions::setPressure ( int new_var ) {
	pressure = new_var;
}

/**
 * Get the value of pressure
 * @return the value of pressure
 */
int ProgramOptions::getPressure ( ) {
	return pressure;
}

/**
 * Set the value of regMarkAuto
 * @param new_var the new value of regMarkAuto
 */
void ProgramOptions::setRegMarkAuto ( bool new_var ) {
	regMarkAuto = new_var;
}

/**
 * Get the value of regMarkAuto
 * @return the value of regMarkAuto
 */
bool ProgramOptions::getRegMarkAuto ( ) {
	return regMarkAuto;
}

/**
 * Set the value of regMark
 * @param new_var the new value of regMark
 */
void ProgramOptions::setRegMark ( bool new_var ) {
	regMark = new_var;
}

/**
 * Get the value of regMark
 * @return the value of regMark
 */
bool ProgramOptions::getRegMark ( ) {
	return regMark;
}

/**
 * Set the value of regDimensionWidth
 * @param new_var the new value of regDimensionWidth
 */
void ProgramOptions::setRegDimensionWidth ( double new_var ) {
	regDimensionWidth = new_var;
}

/**
 * Get the value of regDimensionWidth
 * @return the value of regDimensionWidth
 */
double ProgramOptions::getRegDimensionWidth ( ) {
	return regDimensionWidth;
}

/**
 * Set the value of regDimensionHeight
 * @param new_var the new value of regDimensionHeight
 */
void ProgramOptions::setRegDimensionHeight ( double new_var ) {
	regDimensionHeight = new_var;
}

/**
 * Get the value of regDimensionHeight
 * @return the value of regDimensionHeight
 */
double ProgramOptions::getRegDimensionHeight ( ) {
	return regDimensionHeight;
}

/**
 * Set the value of regOriginWidth
 * @param new_var the new value of regOriginWidth
 */
void ProgramOptions::setRegOriginWidth ( double new_var ) {
	regOriginWidth = new_var;
}

/**
 * Get the value of regOriginWidth
 * @return the value of regOriginWidth
 */
double ProgramOptions::getRegOriginWidth ( ) {
	return regOriginWidth;
}

/**
 * Set the value of regOriginHeight
 * @param new_var the new value of regOriginHeight
 */
void ProgramOptions::setRegOriginHeight ( double new_var ) {
	regOriginHeight = new_var;
}

/**
 * Get the value of regOriginHeight
 * @return the value of regOriginHeight
 */
double ProgramOptions::getRegOriginHeight ( ) {
	return regOriginHeight;
}

/**
 * Set the value of version
 * @param new_var the new value of version
 */
void ProgramOptions::setVersion ( QString new_var ) {
	version = new_var;
}

/**
 * Get the value of version
 * @return the value of version
 */
QString ProgramOptions::getVersion ( ) {
	return version;
}

/**
 * Set the value of vendorUSB_ID
 * @param new_var the new value of vendorUSB_ID
 */
void ProgramOptions::setVendorUSB_ID ( int new_var ) {
	vendorUSB_ID = new_var;
}

/**
 * Get the value of vendorUSB_ID
 * @return the value of vendorUSB_ID
 */
int ProgramOptions::getVendorUSB_ID ( ) {
	return vendorUSB_ID;
}

/**
 * Set the value of productUSB_ID
 * @param new_var the new value of productUSB_ID
 */
void ProgramOptions::setProductUSB_ID ( int new_var ) {
	productUSB_ID = new_var;
}

/**
 * Get the value of productUSB_ID
 * @return the value of productUSB_ID
 */
int ProgramOptions::getProductUSB_ID ( ) {
	return productUSB_ID;
}

/**
 * Set the value of marginTop
 * @param new_var the new value of marginTop
 */
void ProgramOptions::setMarginTop ( int new_var ) {
	marginTop = new_var;
}

/**
 * Get the value of marginTop
 * @return the value of marginTop
 */
int ProgramOptions::getMarginTop ( ) {
	return marginTop;
}

/**
 * Set the value of marginRight
 * @param new_var the new value of marginRight
 */
void ProgramOptions::setMarginRight ( int new_var ) {
	marginRight = new_var;
}

/**
 * Get the value of marginRight
 * @return the value of marginRight
 */
int ProgramOptions::getMarginRight ( ) {
	return marginRight;
}

/**
 * Set the value of trackEnhancing
 * @param new_var the new value of trackEnhancing
 */
void ProgramOptions::setTrackEnhancing ( bool new_var ) {
	trackEnhancing = new_var;
}

/**
 * Get the value of trackEnhancing
 * @return the value of trackEnhancing
 */
bool ProgramOptions::getTrackEnhancing ( ) {
	return trackEnhancing;
}

// Other methods
//  


/**
 * @param  argc
 * @param  argv
 */
int ProgramOptions::GetOpt (int argc, char *argv[] )
{
	int index;
	int c;
	opterr = 0;
		while ((c = getopt (argc, argv, "scf:")) != -1)
		{
			switch (c)
			{
			case 's':
				sortPath = true;
				break;
			case 'c':
				startCut = true;
				break;
			case 'f':
				fileName = optarg;
				break;
			case '?':
			 if (optopt == 'f')
				cerr << "Option -"<< optopt <<" requires an argument." << endl;
			else if (isprint (optopt))
				cerr << "Unknown option `-"<< optopt <<"'." << endl;
			else
				cerr << "Unknown option character `\\x"<< optopt <<"'." << endl;
				return 1;
			default:
				return 2;
			}
		}
		for (index = optind; index < argc; index++)
		cout << "Non-option argument " << argv[index] <<"'." << endl;
	return 0;
}


/**
 */
void ProgramOptions::showHelp ( )
{
	cout << "The `Robocut' program plots or cuts SVG files on a Craft Robo or Silhouette SD\n";
	cout << "or most likely any other customer grade Graphtec cutting plotter.\n\n";
	cout << "Usage:\n";
	cout << "  Robocut [OPTIONS...] [File]\n\n";
	cout << "Help Options:\n";
	cout << "  --help                  Show summary of options\n";
	cout << "  --version               Show version information and copyright details\n\n";
	cout << "Application Options:\n";
	cout << "  --font <font>           Font to use, default is utopia, to see a list use `xlsfonts' command\n";
	cout << "Report bugs to https://bugs.launchpad.net/robocut/+filebug.\n";
}


/**
 */
void ProgramOptions::showVersion ( )
{
	cout << (version.toStdString());
	cout << "\n\nCopyright (C) 2010\n";
	cout << "This is free software; see the source for copying conditions.  There is NO\n";
	cout << "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\n";
	cout << "Written by Tim Hutt and Markus Schulz <schulz@alpharesearch.de>\n";
}

void ProgramOptions::initAttributes ( ) {
	sortPath = false;
	startCut = false;
	fileName = "";
	tspSortPath = false;
	media = 0;
	speed = 33;
	pressure = 10;
	regMarkAuto = false;
	regMark = false;
	regDimensionWidth = 180.0;
	regDimensionHeight = 240.0;
	regOriginWidth = 15.0;
	regOriginHeight = 10.0;
	vendorUSB_ID = 0x0b4d;
	productUSB_ID = 0x111d;
	marginTop = 500;
	marginRight = 320;
	trackEnhancing = false;
}

