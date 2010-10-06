#pragma once

#include <QList>
#include <QPolygonF>
#include "Common.h"

using namespace std;

// Perform the cut. Returns error state. Note that this just sends the data, it has no way of knowing whether it really
// worked and there is no easy way to cancel it.
//
// media: The type of media. I'm not sure what this effects, see CuttingDialog.cpp/ui for values. 300 is "custom".
//        The value is not fully checked for sanity.
// speed: Cutting speed: 1-10 inclusive.
// pressure: Cutting pressure: 1-33 inclusive.
// trackenhancing: I honestly have no idea.
Error Cut(QList<QPolygonF> cuts, int media = 300, int speed = 10, int pressure = 10, bool trackenhancing = false);
