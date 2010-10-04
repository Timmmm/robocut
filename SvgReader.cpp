#include "SvgReader.h"

#include <QFile>

#include <QXmlStreamReader>
#include <QStringList>
#include <QDebug>


Lines ReadSVG(QString filename)
{
	Lines l;
	QFile file("in.txt");
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
			return l;

	QXmlStreamReader xml(&file);

	QStringList pathStrings;

	while (!xml.atEnd())
	{
		xml.readNext();
		if (xml.isStartElement())
			if (xml.name() == "path")
				pathStrings.append(xml.attributes().value("d").toString());
	}
	if (xml.error() && xml.error() != QXmlStreamReader::PrematureEndOfDocumentError)
	{
		qWarning() << "XML ERROR:" << xml.lineNumber() << ": " << xml.errorString();
		return l;
	}

	// Now parse the strings.
//	for
	return l;
}
