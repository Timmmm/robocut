#pragma once

#include <vector>
#include <QString>

using namespace std;

// A list of lines. Each line consists of a series of x,y pairs.
typedef vector<vector<double> > Lines;

Lines ReadSVG(QString filename);
