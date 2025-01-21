#ifndef CONFIG_H
#define CONFIG_H

#include <cstdint>
#include <vector>
#include <string>

// Bitmap file header structure
#pragma pack(push, 1)
struct BMPFileHeader {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
};

// Bitmap info header structure
struct BMPInfoHeader {
    uint32_t biSize;
    int32_t biWidth;
    int32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t biXPelsPerMeter;
    int32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
};

// Structure to represent an RGB color
struct RGB {
    uint8_t r;
    uint8_t g;
    uint8_t b;

    bool operator==(RGB& n) {
        return (this->r == n.r && this->g == n.g && this->b == n.b);
    }
};

struct ColorEntry {
    RGB color;
    int x, y;
};


class Image {
private:
    std::string inputPath;
    std::string palleteImagePath;
    std::string biasCustomValue;
    std::string searchTermSuffix;
    std::string imageDownloadPath;
    std::string scaledImagePath;
    std::string finalProcessedImagePath;
    int newWidth;
    int newHeight;
    int cellWidthMSPaint;
    int cellHeightMSPaint;

public:
    // Parameterized constructor
    Image(
        const std::string& inputPath = "input.bmp",
        const std::string& palleteImagePath = "pallete.png",
        const std::string& biasCustomValue = "15",
        const std::string& searchTermSuffix = " white background",
        const std::string& imageDownloadPath = "download.bmp",
        const std::string& scaledImagePath = "image_scaled.bmp",
        const std::string& finalProcessedImagePath = "image_20bit.bmp",
        int newWidth = 500,
        int newHeight = 352,
        int cellWidthMSPaint = 22,
        int cellHeightMSPaint = 22
    ) : inputPath(inputPath),
        palleteImagePath(palleteImagePath),
        biasCustomValue(biasCustomValue),
        searchTermSuffix(searchTermSuffix),
        imageDownloadPath(imageDownloadPath),
        scaledImagePath(scaledImagePath),
        finalProcessedImagePath(finalProcessedImagePath),
        newWidth(newWidth),
        newHeight(newHeight),
        cellWidthMSPaint(cellWidthMSPaint),
        cellHeightMSPaint(cellHeightMSPaint) {
    }

    // Getters
    std::string getInputPath() const { return inputPath; }
    std::string getPalleteImagePath() const { return palleteImagePath; }
    std::string getBiasCustomValue() const { return biasCustomValue; }
    std::string getSearchTermSuffix() const { return searchTermSuffix; }
    std::string getImageDownloadPath() const { return imageDownloadPath; }
    std::string getScaledImagePath() const { return scaledImagePath; }
    std::string getFinalProcessedImagePath() const { return finalProcessedImagePath; }
    int getNewWidth() const { return newWidth; }
    int getNewHeight() const { return newHeight; }
    int getCellWidthMSPaint() const { return cellWidthMSPaint; }
    int getCellHeightMSPaint() const { return cellHeightMSPaint; }

    // Setters
    void setInputPath(const std::string& path) { inputPath = path; }
    void setPalleteImagePath(const std::string& path) { palleteImagePath = path; }
    void setBiasCustomValue(const std::string& value) { biasCustomValue = value; }
    void setSearchTermSuffix(const std::string& suffix) { searchTermSuffix = suffix; }
    void setImageDownloadPath(const std::string& path) { imageDownloadPath = path; }
    void setScaledImagePath(const std::string& path) { scaledImagePath = path; }
    void setFinalProcessedImagePath(const std::string& path) { finalProcessedImagePath = path; }
    void setNewWidth(int width) { newWidth = width; }
    void setNewHeight(int height) { newHeight = height; }
    void setCellWidthMSPaint(int width) { cellWidthMSPaint = width; }
    void setCellHeightMSPaint(int height) { cellHeightMSPaint = height; }
};

// Define the color palette in MS Paint ({RGB}, x_pos, y_pos)
static std::vector<ColorEntry> colorPalette = {
    {{0, 0, 0}, 0, 0},
    {{127, 127, 127}, 0, 1},
    {{136, 0, 21}, 0, 2},
    {{237, 28, 36}, 0, 3},
    {{255, 127, 39}, 0, 4},
    {{255, 242, 0}, 0, 5},
    {{34, 177, 76}, 0, 6},
    {{0, 162, 232}, 0, 7},
    {{63, 72, 204}, 0, 8},
    {{163, 73, 164}, 0, 9},
    {{255, 255, 255}, 1, 0},
    {{195, 195, 195}, 1, 1},
    {{185, 122, 87}, 1, 2},
    {{255, 174, 201}, 1, 3},
    {{255, 201, 14}, 1, 4},
    {{239, 228, 176}, 1, 5},
    {{181, 230, 29}, 1, 6},
    {{153, 217, 234}, 1, 7},
    {{112, 146, 190}, 1, 8},
    {{200, 191, 231}, 1, 9},
};


#pragma pack(pop)

#endif // BITMAP_STRUCTS_H
