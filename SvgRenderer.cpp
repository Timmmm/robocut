#include "SvgRenderer.h"

#include <QPainter>
#include <QDebug>
#include <QApplication>
#include <QPalette>

#include "PathPaintDevice.h"

namespace
{
// Parse a width or height attribute, and try to return its size in mm.
// If no units are given it is assumed to be mm.
// If the unit is percent, then it returns 0.
// length ::= number ("em" | "ex" | "px" | "in" | "cm" | "mm" | "pt" | "pc" | "%")?
double sizeAttributeToMm(const QString& attr)
{
	double multiplier = 1.0;

	auto a = attr.trimmed();

	if (a.endsWith("em"))
	{
		multiplier = 1.0;
		a.chop(2);
	}
	else if (a.endsWith("ex"))
	{
		multiplier = 1.0;
		a.chop(2);
	}
	else if (a.endsWith("px"))
	{
		multiplier = 1.0;
		a.chop(2);
	}
	else if (a.endsWith("in"))
	{
		multiplier = 1.0;
		a.chop(2);
	}
	else if (a.endsWith("cm"))
	{
		multiplier = 1.0;
		a.chop(2);
	}
	else if (a.endsWith("mm"))
	{
		multiplier = 1.0;
		a.chop(2);
	}
	else if (a.endsWith("pt"))
	{
		multiplier = 1.0;
		a.chop(2);
	}
	else if (a.endsWith("pc"))
	{
		multiplier = 1.0;
		a.chop(2);
	}
	else if (a.endsWith("%"))
	{
		return 0.0;
	}

	bool ok = false;
	double len = a.toDouble(&ok);
	if (!ok)
		return 0.0;

	return len * multiplier;
}

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

} // anonymous namespace

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
	
	render.widthMm = sizeAttributeToMm(render.widthAttribute);
	render.heightMm = sizeAttributeToMm(render.heightAttribute);
	
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
	
	// These are the paths in user units.
	render.paths = pg.paths();
	
	render.success = true;
	
	
	if (render.widthMm <= 0.0 || render.heightMm <= 0.0)
	{
		render.widthMm = render.viewBox.width();
		render.heightMm = render.viewBox.height();
	}
	
	return render;
}

QPixmap svgToPreviewImage(const QString& filename, QSize dimensions)
{
	auto background = QApplication::palette("QWidget").color(QPalette::Base);
	auto foreground = QApplication::palette("QWidget").color(QPalette::Text);

	QPixmap pix(dimensions);
	pix.fill(background);

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
	painter.setPen(QPen(foreground));
	painter.setTransform(transform);
	
	// Work out the scaling and offset for the paths.
	for (const auto& path : paths.paths)
		painter.drawPolygon(path);

	return pix;	
}
