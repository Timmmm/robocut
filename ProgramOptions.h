
#ifndef PROGRAMOPTIONS_H
#define PROGRAMOPTIONS_H

#include <QString>
class ProgramOptions
{
public:

	// Constructors/Destructors
	//  


	/**
	 * Empty Constructor
	 */
	ProgramOptions ( );

	/**
	 * Empty Destructor
	 */
	virtual ~ProgramOptions ( );



	/**
	 * @param  argc
	 * @param  argv
	 * @return the value
	 */
	int GetOpt (int argc, char *argv[] );

protected:

public:

protected:

public:

protected:


private:

	// Private attributes
	//  

	bool sortPath;
	bool startCut;
	QString fileName;
	bool tspSortPath;
	int media;
	int speed;
	int pressure;
	bool regMarkAuto;
	bool regMark;
	double regDimensionWidth;
	double regDimensionHeight;
	double regOriginWidth;
	double regOriginHeight;
	QString version;
	int vendorUSB_ID;
	int productUSB_ID;
	int marginTop;
	int marginRight;
	bool trackEnhancing;
public:

private:

public:


	// Private attribute accessor methods
	//  


	/**
	 * Set the value of sortPath
	 * @param new_var the new value of sortPath
	 */
	void setSortPath ( bool new_var );

	/**
	 * Get the value of sortPath
	 * @return the value of sortPath
	 */
	bool getSortPath ( );


	/**
	 * Set the value of startCut
	 * @param new_var the new value of startCut
	 */
	void setStartCut ( bool new_var );

	/**
	 * Get the value of startCut
	 * @return the value of startCut
	 */
	bool getStartCut ( );


	/**
	 * Set the value of fileName
	 * @param new_var the new value of fileName
	 */
	void setFileName ( QString new_var );

	/**
	 * Get the value of fileName
	 * @return the value of fileName
	 */
	QString getFileName ( );


	/**
	 * Set the value of tspSortPath
	 * @param new_var the new value of tspSortPath
	 */
	void setTspSortPath ( bool new_var );

	/**
	 * Get the value of tspSortPath
	 * @return the value of tspSortPath
	 */
	bool getTspSortPath ( );


	/**
	 * Set the value of media
	 * @param new_var the new value of media
	 */
	void setMedia ( int new_var );

	/**
	 * Get the value of media
	 * @return the value of media
	 */
	int getMedia ( );


	/**
	 * Set the value of speed
	 * @param new_var the new value of speed
	 */
	void setSpeed ( int new_var );

	/**
	 * Get the value of speed
	 * @return the value of speed
	 */
	int getSpeed ( );


	/**
	 * Set the value of pressure
	 * @param new_var the new value of pressure
	 */
	void setPressure ( int new_var );

	/**
	 * Get the value of pressure
	 * @return the value of pressure
	 */
	int getPressure ( );


	/**
	 * Set the value of regMarkAuto
	 * @param new_var the new value of regMarkAuto
	 */
	void setRegMarkAuto ( bool new_var );

	/**
	 * Get the value of regMarkAuto
	 * @return the value of regMarkAuto
	 */
	bool getRegMarkAuto ( );


	/**
	 * Set the value of regMark
	 * @param new_var the new value of regMark
	 */
	void setRegMark ( bool new_var );

	/**
	 * Get the value of regMark
	 * @return the value of regMark
	 */
	bool getRegMark ( );


	/**
	 * Set the value of regDimensionWidth
	 * @param new_var the new value of regDimensionWidth
	 */
	void setRegDimensionWidth ( double new_var );

	/**
	 * Get the value of regDimensionWidth
	 * @return the value of regDimensionWidth
	 */
	double getRegDimensionWidth ( );


	/**
	 * Set the value of regDimensionHeight
	 * @param new_var the new value of regDimensionHeight
	 */
	void setRegDimensionHeight ( double new_var );

	/**
	 * Get the value of regDimensionHeight
	 * @return the value of regDimensionHeight
	 */
	double getRegDimensionHeight ( );


	/**
	 * Set the value of regOriginWidth
	 * @param new_var the new value of regOriginWidth
	 */
	void setRegOriginWidth ( double new_var );

	/**
	 * Get the value of regOriginWidth
	 * @return the value of regOriginWidth
	 */
	double getRegOriginWidth ( );


	/**
	 * Set the value of regOriginHeight
	 * @param new_var the new value of regOriginHeight
	 */
	void setRegOriginHeight ( double new_var );

	/**
	 * Get the value of regOriginHeight
	 * @return the value of regOriginHeight
	 */
	double getRegOriginHeight ( );


	/**
	 * Set the value of version
	 * @param new_var the new value of version
	 */
	void setVersion ( QString new_var );

	/**
	 * Get the value of version
	 * @return the value of version
	 */
	QString getVersion ( );


	/**
	 * Set the value of vendorUSB_ID
	 * @param new_var the new value of vendorUSB_ID
	 */
	void setVendorUSB_ID ( int new_var );

	/**
	 * Get the value of vendorUSB_ID
	 * @return the value of vendorUSB_ID
	 */
	int getVendorUSB_ID ( );


	/**
	 * Set the value of productUSB_ID
	 * @param new_var the new value of productUSB_ID
	 */
	void setProductUSB_ID ( int new_var );

	/**
	 * Get the value of productUSB_ID
	 * @return the value of productUSB_ID
	 */
	int getProductUSB_ID ( );


	/**
	 * Set the value of marginTop
	 * @param new_var the new value of marginTop
	 */
	void setMarginTop ( int new_var );

	/**
	 * Get the value of marginTop
	 * @return the value of marginTop
	 */
	int getMarginTop ( );


	/**
	 * Set the value of marginRight
	 * @param new_var the new value of marginRight
	 */
	void setMarginRight ( int new_var );

	/**
	 * Get the value of marginRight
	 * @return the value of marginRight
	 */
	int getMarginRight ( );


	/**
	 * Set the value of trackEnhancing
	 * @param new_var the new value of trackEnhancing
	 */
	void setTrackEnhancing ( bool new_var );

	/**
	 * Get the value of trackEnhancing
	 * @return the value of trackEnhancing
	 */
	bool getTrackEnhancing ( );

private:



	/**
	 */
	void showHelp ( );


	/**
	 */
	void showVersion ( );

	void initAttributes ( ) ;

};

#endif // PROGRAMOPTIONS_H
