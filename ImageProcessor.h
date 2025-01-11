#pragma once

#include "bitmap_structs.h"
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

class ImageProcessor {
private:
    RGB findClosestColor(const RGB& pixel, const std::vector<ColorEntry>& colorPalette);
    void swapRGBToBGR(unsigned char* imageData, int width, int height);

public:
    cv::Mat captureScreen();
    void startDrawing(const cv::Mat& image, AutoClicker& ac, int brush_size = 4);
    void scaleBMP(const std::string& inputPath, const std::string& outputPath, int newWidth, int newHeight);
    void reduceImageColors(const std::string& inputPath, const std::string& outputPath, const std::vector<ColorEntry>& colorPalette);
    bool saveAsBMP(const std::string& filename, unsigned char* imageData, int width, int height);

    struct Vec3bHash {
        std::size_t operator()(const cv::Vec3b& color) const;
    };

    void splitImageByColorsAndStartDrawing(const std::string& inputFile, AutoClicker& ac, std::vector<ColorEntry> colorPallete, int cellWidth = 20, int cellHeight = 20, int bias = 10);
    cv::Point locateImageOnScreen(const cv::Mat& screenImage, const cv::Mat& templateImage);
};
