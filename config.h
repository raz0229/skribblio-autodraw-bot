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


static std::string inputPath = "input.bmp";
static std::string palleteImagePath = "pallete.png";
static std::string biasCustomValue = "15";
static std::string searchTermSuffix = " white background";  // Add "white background" to every searh query
static std::string imageDownloadPath = "download.bmp";
static std::string scaledImagePath = "image_scaled.bmp";
static std::string finalProcessedImagePath = "image_20bit.bmp";
static int newWidth = 500; // Width of scaled Image
static int newHeight = 352; // Height of scaled Image
static int cellWidthMSPaint = 22;
static int cellHeightMSPaint = 22;

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
