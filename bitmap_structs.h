#ifndef BITMAP_STRUCTS_H
#define BITMAP_STRUCTS_H

#include <cstdint>

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

#pragma pack(pop)

#endif // BITMAP_STRUCTS_H
