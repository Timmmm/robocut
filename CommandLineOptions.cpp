#include "CommandLineOptions.h"

#include <QCommandLineParser>

enum CommandLineParseResult
{
	CommandLineOk,
	CommandLineError,
	CommandLineVersionRequested,
	CommandLineHelpRequested
};

struct CommandLineOptions
{
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
};
/*
CommandLineParseResult parseCommandLine(QCommandLineParser& parser,
                                        CommandLineOptions& result,
                                        QString& errorMessage)
{
	
	// -h, --help
	
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
	
	
	
	
	
	
	
	
	parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
	
	// {flag(s), Help, Variable Name, Default Value}
	parser.addOptions({
	                      // Sort mode (?, ?, ?)
	                      {"sort", "Sort mode (auto, none, ?)", "sort", "auto"},
	                      
	                      // Media (int)
	                      {"media", "Media type, an integer choosing the option from the drop-down starting at 0", "media", "0"},            
	                      
	                      // Margins (top, right)
	                      {"margins", "Left and right margins in mm, e.g. 25.4,50.8", "margins", "0,0"},
	                      
	                      // Registration mark mode (auto / manual / none)
	                      {"registration", "Registration mark mode: auto, manual or none", "registration", "none"},
	                      
	                      // Registration x, y, width, height
	                      {"registration_positions", "Registration positions as: x, y, width, height", "registration_positions", "0,0,0,0"},
	                      
	                      // USB vendor ID, product id.
	                      {"usb_ids", "USB vendor ID and product ID in hex, e.g. 234b,8af0", "usb_ids", ""},
	                      
	                      // Track enhancing
	                      {"track_enhancing", "Feed the media in and out a few times to press tracks into it before cutting", "track_enhancing"},
	                      
	                      // Speed
	                      {"speed", "Speed between 1 and 33", "speed", ""},
	                      
	                      // Pressure
	                      {"pressure", "Pressure between 1 and 10", "pressure", ""},
	                      
	                      // Run mode
	                      {"mode", "How to start the program - gui,cut,cut_dialog", "mode", "gui"},
	                  });
	
	QCommandLineOption helpOption = parser.addHelpOption();
	QCommandLineOption versionOption = parser.addVersionOption();
	
	if (!parser.parse(QCoreApplication::arguments())) {
		*errorMessage = parser.errorText();
		return CommandLineError;
	}
	
	if (parser.isSet(versionOption))
		return CommandLineVersionRequested;
	
	if (parser.isSet(helpOption))
		return CommandLineHelpRequested;
	
	if (parser.isSet(nameServerOption)) {
		const QString nameserver = parser.value(nameServerOption);
		query->nameServer = QHostAddress(nameserver);
		if (query->nameServer.isNull() || query->nameServer.protocol() == QAbstractSocket::UnknownNetworkLayerProtocol) {
			*errorMessage = "Bad nameserver address: " + nameserver;
			return CommandLineError;
		}
	}
	
	if (parser.isSet(typeOption)) {
		const QString typeParameter = parser.value(typeOption);
		const int type = typeFromParameter(typeParameter.toLower());
		if (type < 0) {
			*errorMessage = "Bad record type: " + typeParameter;
			return CommandLineError;
		}
		query->type = static_cast<QDnsLookup::Type>(type);
	}
	
	const QStringList positionalArguments = parser.positionalArguments();
	if (positionalArguments.isEmpty()) {
		*errorMessage = "Argument 'name' missing.";
		return CommandLineError;
	}
	if (positionalArguments.size() > 1) {
		*errorMessage = "Several 'name' arguments specified.";
		return CommandLineError;
	}
	query->name = positionalArguments.first();
	
	return CommandLineOk;
}
*/
