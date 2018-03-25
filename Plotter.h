#pragma once

#include <QList>
#include <QPolygonF>
#include "Common.h"

using namespace std;

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
Error Cut(CutParams p);


const int VENDOR_ID_GRAPHTEC = 0x0b4d; 
const int PRODUCT_ID_CC200_20 = 0x110a;
const int PRODUCT_ID_CC300_20 = 0x111a;
const int PRODUCT_ID_SILHOUETTE_SD_1 = 0x111c;
const int PRODUCT_ID_SILHOUETTE_SD_2 = 0x111d;
const int PRODUCT_ID_SILHOUETTE_CAMEO =  0x1121;
const int PRODUCT_ID_SILHOUETTE_CAMEO_3 =  0x112f;
const int PRODUCT_ID_SILHOUETTE_PORTRAIT = 0x1123;

struct cutter_id
{
  string msg;
  int usb_vendor_id;
  int usb_product_id;
};

struct cutter_id *Identify();
