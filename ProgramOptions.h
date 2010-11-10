
#ifndef PROGRAMOPTIONS_H
#define PROGRAMOPTIONS_H

#include <QString>
class ProgramOptions
{

private:
	ProgramOptions ( );
	ProgramOptions(const ProgramOptions &);
	ProgramOptions & operator=(const ProgramOptions &);
	virtual ~ProgramOptions ( );

public:
	// Singleton
	static ProgramOptions& Instance();

private:
	int sortPath;
	int startCut;
	QString fileName;
	int tspSortPath;
	int media;
	int speed;
	int pressure;
	int regMarkAuto;
	int regMark;
	double regDimensionWidth;
	double regDimensionHeight;
	double regOriginWidth;
	double regOriginHeight;
	QString version;
	int vendorUSB_ID;
	int productUSB_ID;
	double marginTop;
	double marginRight;
	int trackEnhancing;

public:
	void setSortPath ( int new_var );
	int getSortPath ( );
	void setStartCut ( int new_var );
	int getStartCut ( );
	void setFileName ( QString new_var );
	QString getFileName ( );
	void setTspSortPath ( int new_var );
	int getTspSortPath ( );
	void setMedia ( int new_var );
	int getMedia ( );
	void setSpeed ( int new_var );
	int getSpeed ( );
	void setPressure ( int new_var );
	int getPressure ( );
	void setRegMarkAuto ( int new_var );
	int getRegMarkAuto ( );
	void setRegMark ( int new_var );
	int getRegMark ( );
	void setRegDimensionWidth ( int new_var );
	int getRegDimensionWidth ( );
	void setRegDimensionHeight ( int new_var );
	int getRegDimensionHeight ( );
	void setRegOriginWidth ( int new_var );
	int getRegOriginWidth ( );
	void setRegOriginHeight ( int new_var );
	int getRegOriginHeight ( );
	void setRegDimensionWidthMM ( double new_var );
	double getRegDimensionWidthMM ( );
	void setRegDimensionHeightMM ( double new_var );
	double getRegDimensionHeightMM ( );
	void setRegOriginWidthMM ( double new_var );
	double getRegOriginWidthMM ( );
	void setRegOriginHeightMM ( double new_var );
	double getRegOriginHeightMM ( );
	void setVersion ( QString new_var );
	QString getVersion ( );
	void setVendorUSB_ID ( int new_var );
	int getVendorUSB_ID ( );
	void setProductUSB_ID ( int new_var );
	int getProductUSB_ID ( );
	void setMarginTop ( int new_var );
	int getMarginTop ( );
	void setMarginRight ( int new_var );
	int getMarginRight ( );
	void setMarginTopMM ( double new_var );
	double getMarginTopMM ( );
	void setMarginRightMM ( double new_var );
	double getMarginRightMM ( );
	void setTrackEnhancing ( int new_var );
	int getTrackEnhancing ( );

private:
	void showHelp ( );
	void showVersion ( );
	void showShow ( );
	void initAttributes ( ) ;
	
public:
	int GetOpt (int argc, char *argv[] );
};

#endif // PROGRAMOPTIONS_H
