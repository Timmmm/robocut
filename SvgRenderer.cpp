#include "SvgRenderer.h"

#include <optional>

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QPainter>
#include <QPalette>

#include "PathPaintDevice.h"

namespace
{

// 1 px is 1/96th of an inch.
const double MM_PER_CM = 10.0;
const double MM_PER_Q = 0.25;
const double MM_PER_IN = 25.4;
const double MM_PER_PC = 25.4 / 6.0;
const double MM_PER_PT = 25.4 / 72.0;
const double MM_PER_PX = 25.4 / 96.0;

// Parse a width or height attribute, and try to return its size in mm.
// If no units are given then they are assumed to be px.
// If the unit is not a known absolute unit it returns `none`.
//
// Supported absolute units are "cm", "mm", "Q", "in", "pc", "pt", "px".
//
// See https://www.w3.org/TR/css-values-3/#absolute-lengths
std::optional<double> sizeAttributeToMm(const QString& attr)
{
	// Default with no unit.
	double multiplier = MM_PER_PX;

	auto a = attr.trimmed();

	if (a.endsWith("cm"))
	{
		multiplier = MM_PER_CM;
		a.chop(2);
	}
	else if (a.endsWith("mm"))
	{
		multiplier = 1.0;
		a.chop(2);
	}
	else if (a.endsWith("Q"))
	{
		multiplier = MM_PER_Q;
		a.chop(1);
	}
	else if (a.endsWith("in"))
	{
		multiplier = MM_PER_IN;
		a.chop(2);
	}
	else if (a.endsWith("pc"))
	{
		multiplier = MM_PER_PC;
		a.chop(2);
	}
	else if (a.endsWith("pt"))
	{
		multiplier = MM_PER_PT;
		a.chop(2);
	}
	else if (a.endsWith("px"))
	{
		multiplier = MM_PER_PX;
		a.chop(2);
	}

	// QString::toDouble() is actually sensibly designed (unlike all of the standard library
	// conversions) so that it fails on extra characters (whitespace, trailing letters, etc).
	// This means if there's a unit we didn't handle above it will fail.
	bool ok = false;
	double len = a.toDouble(&ok);
	if (!ok)
		return std::nullopt;

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
		case QXmlStreamReader::StartElement: {
			const auto& attr = xml.attributes();
			if (xml.name() == u"svg")
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
			else if (xml.name() == u"tspan")
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

SResult<SvgRender> svgToPaths(const QString& filename, bool searchForTspans)
{
	SvgRender render;

	QFile svgFile(filename);

	if (!svgFile.open(QIODevice::ReadOnly))
		return ("Couldn't open: " + filename + ": " + svgFile.errorString()).toStdString();

	// Read the entire contents of the file into memory.
	auto svgContents = svgFile.readAll();

	svgFile.close();

	if (svgContents.isEmpty())
		return ("Empty file or error: " + filename).toStdString();

	auto xmlData = scanSvgElements(svgContents, searchForTspans);
	if (xmlData.parseError)
		return "XML parse error";

	render.widthAttribute = xmlData.widthAttribute;
	render.heightAttribute = xmlData.heightAttribute;
	render.hasTspanPosition = xmlData.hasTspanPosition;

	QSvgRenderer renderer;
	if (!renderer.load(svgContents))
		return ("Couldn't render SVG: " + filename).toStdString();

	render.viewBox = renderer.viewBoxF();

	// So here we pretend that the viewbox is in mm.
	PathPaintDevice pg(render.viewBox.width(), render.viewBox.height());
	QPainter p(&pg);

	renderer.render(&p, render.viewBox);

	// These are the paths in user units.
	render.paths = pg.paths();

	// If no width or height were provided then use the viewBox. The units are
	// assumed to be px. See https://svgwg.org/svg2-draft/coords.html#ViewportSpace
	render.widthMm = sizeAttributeToMm(render.widthAttribute).value_or(render.viewBox.width() * MM_PER_PX);
	render.heightMm = sizeAttributeToMm(render.heightAttribute).value_or(render.viewBox.height() * MM_PER_PX);

	return render;
}

QPixmap svgToPreviewImage(const QString& filename, QSize dimensions)
{
	auto background = QApplication::palette("QWidget").color(QPalette::Base);
	auto foreground = QApplication::palette("QWidget").color(QPalette::Text);

	QPixmap pix(dimensions);
	pix.fill(background);

	auto pathsResult = svgToPaths(filename, false);
	if (pathsResult.is_err())
	{
		return pix;
	}

	const auto& paths = pathsResult.unwrap();

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
