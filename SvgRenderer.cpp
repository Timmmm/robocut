#include "SvgRenderer.h"

#include <QPainter>
#include <QDebug>

#include "PathPaintDevice.h"

struct SvgXmlData
{
	QString widthAttribute;
	QString heightAttribute;
	
	bool hasTspanPosition = false;

	bool parseError = true;
};

// Scan the SVG for the <svg> element and extract its width and height attributes.
// If searchForTspans is true then it also searches the entire document for
// tspan elements that contain x or y attributes and sets hasTspanPosition to true if so.
SvgXmlData scanSvgElements(const QByteArray& svgContents, bool searchForTspans)
{
	SvgXmlData data;
	
	// Read the width/height attributes, and also check for tspans.
	QXmlStreamReader xml(svgContents);
	while (!xml.atEnd())
	{
		switch (xml.readNext())
		{
		case QXmlStreamReader::StartElement:
		{
			const auto& attr = xml.attributes();
			if (xml.name() == "svg")
			{
				const auto& width = attr.value("width");
				const auto& height = attr.value("height");
				
				if (!width.isEmpty() && !height.isEmpty())
				{
					data.widthAttribute = width.toString();
					data.heightAttribute = height.toString();
				}
				
				if (!searchForTspans)
				{
					data.parseError = false;
					return data;
				}
			}
			else if (xml.name() == "tspan")
			{
				if (attr.hasAttribute("x") || attr.hasAttribute("y"))
				{
					data.hasTspanPosition = true;
					data.parseError = false;
					return data;
				}
			}
			
			break;
		}
		default:
			break;
		}
	}
	if (xml.hasError())
	{
		qDebug() << "SVG parse error:" << xml.errorString();
	}
	else
	{
		data.parseError = false;
	}
	return data;
}

SvgRender svgToPaths(const QString& filename, bool searchForTspans)
{
	SvgRender render;
	
	QFile svgFile(filename);
	
	if (!svgFile.open(QIODevice::ReadOnly))
	{
		qDebug() << "Couldn't open:" << filename << svgFile.errorString();
        return render;
	}
	
	// Read the entire contents of the file into memory.
	auto svgContents = svgFile.readAll();
	
	svgFile.close();
	
	if (svgContents.isEmpty())
	{
		qDebug() << "Empty file or error:" << filename;
		return render;
	}
	

	auto xmlData = scanSvgElements(svgContents, searchForTspans);
	if (xmlData.parseError)
	{
		qDebug() << "XML parse error";
		return render;
	}
	render.widthAttribute = xmlData.widthAttribute;
	render.heightAttribute = xmlData.heightAttribute;
	render.hasTspanPosition = xmlData.hasTspanPosition;
	
	QSvgRenderer renderer;
	if (!renderer.load(svgContents))
	{
		qDebug() << "Couldn't render SVG:" << filename;
		return render;
	}
	
	render.viewBox = renderer.viewBoxF();

	// So here we pretend that the viewbox is in mm.
	PathPaintDevice pg(render.viewBox.width(), render.viewBox.height());
	QPainter p(&pg);

	renderer.render(&p, render.viewBox);
	
	render.paths = pg.paths();
	
	render.success = true;
	
	return render;
}

QPixmap svgToPreviewImage(const QString& filename, QSize dimensions)
{
	QPixmap pix(dimensions);
	pix.fill();
		
	auto paths = svgToPaths(filename, false);
	
	auto viewBox = paths.viewBox;
	
	QTransform transform;
	
	auto scaleFitWidth = dimensions.width() / viewBox.width();
	auto scaleFitHeight = dimensions.height() / viewBox.height();
	
	if (scaleFitHeight < scaleFitWidth)
	{
		// Translate for the padding bar.
		transform.translate((dimensions.width() - (viewBox.width() * scaleFitHeight)) / 2.0, 0.0);

		// Scale to fit the height, with padding bars on the left and right
		transform.scale(scaleFitHeight, scaleFitHeight);
	}
	else
	{

		// Translate for the padding bar.
		transform.translate(0.0, (dimensions.height() - (viewBox.height() * scaleFitWidth)) / 2.0);
		
		// Scale to fit the width, with padding bars on the top and bottom.
		transform.scale(scaleFitWidth, scaleFitWidth);
	}

	transform.translate(-viewBox.left(), -viewBox.top());	
	
	QPainter painter(&pix);
	painter.setBrush(Qt::NoBrush);
	painter.setPen(QPen(Qt::black));
	painter.setTransform(transform);
	
	// Work out the scaling and offset for the paths.
	for (const auto& path : paths.paths)
		painter.drawPolygon(path);

	return pix;	
}
