#include "HPGL2.h"

#include <sstream>

const double mm2hpgl = 40.0;

std::string renderToHPGL2(const QList<QPolygonF> &paths, double widthMm, double heightMm)
{
	std::stringstream ss;

	// HPGL can't handle 1e-17 etc.
	ss.flags(std::ios::fixed);

	// Initialise.
	ss << "IN;";
	// Set soft clipping window to the entire page.
	ss << "IW0,0," << lroundl(widthMm * mm2hpgl) << "," << lroundl(heightMm * mm2hpgl) << ";";
	// Select Pen 1. This is necessary.
	ss << "SP1;";

	// Pen Width. In mm by default I think.
	ss << "PW" << 1 << ";";

	// TODO: There is a binary encoding available.

	for (const auto &path : paths)
	{
		for (unsigned i = 0; i < path.size(); ++i) {
			// Pen Up/Down (move to/draw to).
			ss << (i == 0 ? "PU" : "PD") << path[i].x() * mm2hpgl << "," << path[i].y() * mm2hpgl << ";";
		}
	}

	// Page feed. Starts the rasterisation/printing.
	ss << "PG;";
	return ss.str();
}
