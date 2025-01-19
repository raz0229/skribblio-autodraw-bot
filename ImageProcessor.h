#pragma once

#include "config.h"
#include "third-party/stb_image.h"
#include <windows.h>
#include <gdiplus.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <cmath>
#include <wininet.h>
#include <string>
#include <unordered_map>
#include <opencv2/opencv.hpp>
#include "AutoClicker.h"

class ImageProcessor : public Image {
private:
    RGB findClosestColor(const RGB& pixel, const std::vector<ColorEntry>& colorPalette);
    void swapRGBToBGR(unsigned char* imageData, int width, int height);

public:
    cv::Mat captureScreen();
    void startDrawing(const cv::Mat& image, AutoClicker& ac, POINT clickPosition, int brush_size = 4);
    void scaleBMP();
    void reduceImageColors(const std::vector<ColorEntry>& colorPalette);
    bool saveAsBMP(unsigned char* imageData, int width, int height);

    struct Vec3bHash {
        std::size_t operator()(const cv::Vec3b& color) const;
    };

    void splitImageByColorsAndStartDrawing(AutoClicker& ac, std::vector<ColorEntry> colorPallete, POINT clickPosition, int bias = 14);
    cv::Point locateImageOnScreen(const cv::Mat& screenImage, const cv::Mat& templateImage);
};
