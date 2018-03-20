#!/bin/bash

# A little script to generate the icon files from a 1024x1024 PNG called Robocut.png
# for Windows and Mac. This script only runs on Mac.

# Windows ICO
mkdir Robocut_ico
sips -z 16 16     Robocut.png --out Robocut_ico/icon_16.png
sips -z 32 32     Robocut.png --out Robocut_ico/icon_32.png
sips -z 48 48     Robocut.png --out Robocut_ico/icon_48.png
sips -z 256 256   Robocut.png --out Robocut_ico/icon_256.png
convert Robocut_ico/* Robocut.ico

# OSX Icon
mkdir Robocut.iconset
sips -z 16 16     Robocut.png --out Robocut.iconset/icon_16x16.png
sips -z 32 32     Robocut.png --out Robocut.iconset/icon_16x16@2x.png
sips -z 32 32     Robocut.png --out Robocut.iconset/icon_32x32.png
sips -z 64 64     Robocut.png --out Robocut.iconset/icon_32x32@2x.png
sips -z 128 128   Robocut.png --out Robocut.iconset/icon_128x128.png
sips -z 256 256   Robocut.png --out Robocut.iconset/icon_128x128@2x.png
sips -z 256 256   Robocut.png --out Robocut.iconset/icon_256x256.png
sips -z 512 512   Robocut.png --out Robocut.iconset/icon_256x256@2x.png
sips -z 512 512   Robocut.png --out Robocut.iconset/icon_512x512.png
sips -z 1024 1024 Robocut.png --out Robocut.iconset/icon_512x512@2x.png
iconutil -c icns Robocut.iconset
