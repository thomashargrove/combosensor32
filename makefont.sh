#!/bin/bash

FILEBASE=LiberationMono-Bold
SIZE=9
~/Documents/arduino/libraries/Adafruit_GFX_Library/fontconvert/fontconvert /Library/Fonts/${FILEBASE}.ttf ${SIZE} > ${FILEBASE}${SIZE}pt7b.h
