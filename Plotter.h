#pragma once

#include <QList>
#include <QPolygonF>
#include "Common.h"

using namespace std;

Error Cut(QList<QPolygonF> cuts, int media = 300, int speed = 10, int pressure = 10, bool trackenhancing = false);
