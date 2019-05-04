#pragma once

#include <QList>
#include <QPolygonF>
#include "Common.h"

#include "CuttingThread.h"

// Perform the cut. Returns error state. Note that this just sends the data, it has no way of knowing whether it really
// worked and there is no easy way to cancel it.
//
// media: The type of media. I'm not sure what this effects, see CuttingDialog.cpp/ui for values. 300 is "custom".
//        The value is not fully checked for sanity.
// speed: Cutting speed: 1-10 inclusive.
// pressure: Cutting pressure: 1-33 inclusive.
// trackenhancing: Supposed to feed/unfeed the media a few times to create tracks.
// regmark: Whether to use registration marks for calibration.
// regsearch: ?
// regwidth/height: Distance between the registration marks.
//
// TODO: Apparently you can change the number of registration marks?
SResult<> Cut(CutParams p);

const int VENDOR_ID_GRAPHTEC = 0x0b4d;

// Map from product ID to product name.
const std::map<int, std::string> PRODUCT_ID_LIST = {
    { 0x110a, "CC200-20" },
    { 0x111a, "CC300-20" },
    { 0x111c, "Silhouette SD / CC220-30" },
    { 0x111d, "Silhouette SD 2" },
    { 0x1121, "Silhouette Cameo" },
    { 0x112f, "Silhouette Cameo 3" },
    { 0x1223, "Silhouette Portrait" },
};

struct CutterId
{
  std::string msg;
  int usb_vendor_id;
  int usb_product_id;
};

