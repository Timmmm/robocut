#include "SvgRenderer.h"

#include <optional>

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QPainter>
#include <QPalette>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

#include <iostream>

#include<unistd.h>

#include "PathPaintDevice.h"

namespace
{

// If the user specifies width="96px" then we assume 96 DPI and return
// a size of 1 inch (this matches the latest versions of Inkscape).
// Note Qt assumes 90 DPI when rendering and SVG but we account for that so
// it has no effect.
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
	QString cutElementId = QString();
	QString markElementId = QString();
	double markStroke;

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
			else if ((xml.name() == u"g") && attr.hasAttribute("id") && attr.value("inkscape:label").contains(QString("cut"),Qt::CaseInsensitive))
			{
				data.cutElementId = attr.value("id").toString();
			}
			else if ((xml.name() == u"g") && attr.hasAttribute("id") && attr.value("inkscape:label").contains(QString("regmarks"),Qt::CaseInsensitive))
			{
				data.markElementId = attr.value("id").toString();
			}
			else if ((xml.name() == u"path") && attr.hasAttribute("style") && attr.value("inkscape:label").contains(QString("RightMarkH"),Qt::CaseInsensitive))
			{
				std::cout<< "found mark" << std::endl;
				auto style = attr.value("style").toString();
				QRegularExpression re("stroke-width:(\\d+(\\.\\d+)?)");
				QRegularExpressionMatch match = re.match(style);
				if (match.hasMatch()) {
					data.markStroke = match.captured(1).toDouble();
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
	render.cutElementId = xmlData.cutElementId;
	render.markElementId = xmlData.markElementId;
	render.markStroke = xmlData.markStroke;

	QSvgRenderer renderer;
	if (!renderer.load(svgContents))
		return ("Couldn't render SVG: " + filename).toStdString();

	render.viewBox = renderer.viewBoxF();

	PathPaintDevice pg(render.viewBox.width(), render.viewBox.height());
	QPainter p(&pg);

	if (!render.markElementId.isEmpty()) {
		double x0,y0,x1,y1;
		PathPaintDevice pgmark(render.viewBox.width(), render.viewBox.height());
		QPainter pmark(&pgmark);
		QRectF bound1 = renderer.boundsOnElement(render.markElementId);
		QTransform transf = renderer.transformForElement(render.markElementId);
		render.markPosition = transf.mapRect(bound1);
		renderer.render(&pmark, render.markElementId, render.markPosition);
		render.markPosition.getCoords(&x0, &y0, &x1, &y1);
		render.markPosition.setCoords(x0* MM_PER_PX, y0* MM_PER_PX, x1* MM_PER_PX, y1* MM_PER_PX);
		bool start = true;
		for ( auto polygon : pgmark.paths() ) {
		    for ( QPointF point : polygon ) {
					if (start) {
						render.markOffset = point;
						start = false;
					} else {
						if ((point.x() >= render.markOffset.x()) && (point.y() <= render.markOffset.y()))
							render.markOffset = point;
					}
				}
		}
		render.markOffset = QPointF(
				(render.markOffset.x()+(render.markStroke/2.0)) * MM_PER_PX,
				(render.markOffset.y()+(render.markStroke/2.0)) * MM_PER_PX);
		std::cout<<"markOffset " <<render.markOffset.x() << "," <<render.markOffset.y()<< std::endl;
	}
	if (!render.cutElementId.isEmpty()) {
		PathPaintDevice pgcut(render.viewBox.width(), render.viewBox.height());
		QPainter pcut(&pgcut);
		QRectF bound1 = renderer.boundsOnElement(render.cutElementId);
		QTransform transf = renderer.transformForElement(render.cutElementId);
		QRectF bound2 = transf.mapRect(bound1);
		renderer.render(&pcut, render.cutElementId, bound2);
		renderer.render(&p);
		render.cutpaths = pgcut.paths();
	}
	else {
	// 	//No specific cut layer - Draw all layers - But can ont use regmarks.
		renderer.render(&p);
		render.cutpaths = pg.paths();
		// These are the paths in user units.
	}
	render.showpaths = pg.paths();


	// If no width or height were provided then use the viewBox. The units are
	// assumed to be px. See https://svgwg.org/svg2-draft/coords.html#ViewportSpace
	render.widthMm = sizeAttributeToMm(render.widthAttribute).value_or(render.viewBox.width() * MM_PER_PX);
	render.heightMm = sizeAttributeToMm(render.heightAttribute).value_or(render.viewBox.height() * MM_PER_PX);

	if (render.markElementId.isEmpty()) {
		render.markOffset = QPointF(render.widthMm,0);
	}
	// Transform the paths from user units to mm.
	for (auto& path : render.cutpaths)
	{
		for (auto& vertex : path)
		{
			vertex = QPointF(
					vertex.x() * MM_PER_PX,
					vertex.y() * MM_PER_PX);
		}
	}

	// Transform the paths from user units to mm.
	for (auto& path : render.showpaths)
	{
		for (auto& vertex : path)
		{
			vertex = QPointF(
					(vertex.x() - render.viewBox.x()) * render.widthMm / render.viewBox.width(),
					(vertex.y() - render.viewBox.y()) * render.heightMm / render.viewBox.height());
		}
	}

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
	for (const auto& path : paths.cutpaths)
		painter.drawPolygon(path);

	return pix;
}
