#pragma once
#ifndef AUTOCLICKER_H
#define AUTOCLICKER_H

#include <windows.h>
#include <iostream>
#include <vector>
#include "config.h"

class ImageProcessor;

class AutoClicker {
    // Function to get the pixel color at the current cursor position
    COLORREF getPixelColorUnderCursor();

public:
    // Function to emulate a mouse click at given x, y coordinates
    void emulateMouseClick(int x, int y, bool addDelay = false);
    void selectColorFromPallete(std::vector<ColorEntry> ce, RGB colorToSelect, ImageProcessor& ip, int cellWidth, int cellHeight, int bias = 10);
    bool testColorPalleteAlignment(std::vector<ColorEntry> cp, ImageProcessor& ip, int cellWidth, int cellHeight, int bias = 10);
    static POINT getMouseClickCoords();
    static void openAndMaximizePaint();
    static void FocusConsoleWindow();
};

#endif // !AUTOCLICKER_H
