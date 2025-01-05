/*
    https://www.geeksforgeeks.org/opencv-c-windows-setup-using-visual-studio-2019/
*/
#define STB_IMAGE_IMPLEMENTATION
#include "third-party/stb_image.h"
#include <windows.h>
#include <gdiplus.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdint>
#include <wininet.h>
#include <opencv2/opencv.hpp>

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

    bool operator==(RGB &n) {
        return (this->r == n.r && this->g == n.g && this->b == n.b);
    }
};

struct ColorEntry {
    RGB color;
    int x, y;

};

#pragma pack(pop)

// include gdiplus as linked library
#pragma comment(lib, "gdiplus.lib")

class ImageProcessor;

class AutoClicker {
    // Function to get the pixel color at the current cursor position
    COLORREF getPixelColorUnderCursor() {
        // Retrieve the cursor's position
        POINT cursorPos;
        if (GetCursorPos(&cursorPos)) {
            // Get the device context for the screen
            HDC hdcScreen = GetDC(NULL); // NULL means the entire screen
            if (hdcScreen) {
                // Get the pixel color at the cursor position
                COLORREF color = GetPixel(hdcScreen, cursorPos.x, cursorPos.y);
                // Release the device context
                ReleaseDC(NULL, hdcScreen);
                return color;
            }
        }
        // Return black if unable to retrieve the color
        return RGB(0, 0, 0);
    }

public:
    // Function to emulate a mouse click at given x, y coordinates
    void emulateMouseClick(int x, int y) {
        // Move the cursor to the specified coordinates
        SetCursorPos(x, y);

        // Emulate mouse left button down and up
        mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
        mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
    }

    void selectColorFromPallete(std::vector<ColorEntry> ce, RGB colorToSelect, ImageProcessor& ip, int cellWidth, int cellHeight, int bias=10);
    bool testColorPalleteAlignment(std::vector<ColorEntry> cp, ImageProcessor& ip, int cellWidth, int cellHeight, int bias = 10);

};

class ImageDownloader {
    // Helper function to download data via HTTP using the WinINet API
    bool downloadData(const std::string& url, std::string& response) {
        HINTERNET hInternet = InternetOpen(L"HTTP Client", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
        if (!hInternet) {
            std::cerr << "Failed to initialize WinINet." << std::endl;
            return false;
        }

        HINTERNET hConnect = InternetOpenUrlA(hInternet, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
        if (!hConnect) {
            std::cerr << "Failed to open URL: " << url << std::endl;
            InternetCloseHandle(hInternet);
            return false;
        }

        char buffer[4096];
        DWORD bytesRead;
        while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
            response.append(buffer, bytesRead);
        }

        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return true;
    }
    // Simple function to parse JSON and extract the image URL (not fully robust)
    std::string extractImageUrl(const std::string& json) {
        std::string url;
        size_t start = json.find("\"link\":");
        if (start != std::string::npos) {
            start = json.find("\"", start + 7);  // Skip over the `link` key
            size_t end = json.find("\"", start + 1); // Find the closing quote
            if (start != std::string::npos && end != std::string::npos) {
                url = json.substr(start + 1, end - start - 1);
            }
        }
        return url;
    }
    // Function to download an image from the URL
    bool downloadImage(const std::string& url, const std::string& outputFile) {
        HINTERNET hInternet = InternetOpen(L"HTTP Client", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
        if (!hInternet) {
            std::cerr << "Failed to initialize WinINet." << std::endl;
            return false;
        }

        HINTERNET hConnect = InternetOpenUrlA(hInternet, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
        if (!hConnect) {
            std::cerr << "Failed to open image URL: " << url << std::endl;
            InternetCloseHandle(hInternet);
            return false;
        }

        // Read the image data and save it to a file
        char buffer[4096];
        DWORD bytesRead;
        std::ofstream outFile(outputFile, std::ios::binary);
        if (!outFile.is_open()) {
            std::cerr << "Failed to open file for saving image." << std::endl;
            InternetCloseHandle(hConnect);
            InternetCloseHandle(hInternet);
            return false;
        }

        while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
            outFile.write(buffer, bytesRead);
        }

        outFile.close();
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);

        return true;
    }

public:
    void downloadImageUsingGoogleSearch(const std::string& searchTerm, const std::string& outputFile) {
        try {
            const std::string apiKey = "AIzaSyB4hl9a1RPB_MmuqPm_zNmO49Y20qSf9e4";
            const std::string cx = "7204b6b1decb42058";
            const std::string query = searchTerm + " white background";
            const std::string url = "https://www.googleapis.com/customsearch/v1?q=" + query +
                "&cx=" + cx + "&searchType=image&imgSize=LARGE&safe=high&key=" + apiKey;

            // Step 1: Download the search result JSON
            std::string response;
            if (!downloadData(url, response)) {
                throw std::runtime_error("Failed to download JSON response from Google Custom Search API.");
            }

            // Step 2: Extract the image URL from the JSON response
            std::string imageUrl = extractImageUrl(response);
            if (imageUrl.empty()) {
                throw std::runtime_error("Failed to extract image URL from JSON response.");
            }

            std::cout << "Image URL: " << imageUrl << std::endl;

            // Step 3: Download the image and save it as BMP
            if (!downloadImage(imageUrl, outputFile)) {
                throw std::runtime_error("Failed to download and save the image.");
            }

            std::cout << "Image successfully saved to: " << outputFile << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
};

class ImageProcessor {

    // Function to find the closest color in the palette
    RGB findClosestColor(const RGB& pixel, const std::vector<ColorEntry>& colorPalette) {
        RGB closestColor = colorPalette[0].color;
        int minDistance = INT32_MAX;

        for (const auto& colorWrapper : colorPalette) {
            int distance = (pixel.r - colorWrapper.color.r) * (pixel.r - colorWrapper.color.r) +
                (pixel.g - colorWrapper.color.g) * (pixel.g - colorWrapper.color.g) +
                (pixel.b - colorWrapper.color.b) * (pixel.b - colorWrapper.color.b);
            if (distance < minDistance) {
                minDistance = distance;
                closestColor = colorWrapper.color;
            }
        }
        return closestColor;
    }
    // Function to swap RGB to BGR
    void swapRGBToBGR(unsigned char* imageData, int width, int height) {
        for (int i = 0; i < width * height; ++i) {
            unsigned char* pixel = &imageData[i * 3];
            // Swap red and blue
            unsigned char temp = pixel[0];
            pixel[0] = pixel[2];
            pixel[2] = temp;
        }
    }
   
    



public:
    // Function to capture the screen as a cv::Mat
    cv::Mat captureScreen() {
        HDC hdcScreen = GetDC(NULL);
        HDC hdcMemory = CreateCompatibleDC(hdcScreen);

        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);

        HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, screenWidth, screenHeight);
        SelectObject(hdcMemory, hBitmap);

        BitBlt(hdcMemory, 0, 0, screenWidth, screenHeight, hdcScreen, 0, 0, SRCCOPY);

        BITMAPINFO bmpInfo;
        ZeroMemory(&bmpInfo, sizeof(BITMAPINFO));
        bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmpInfo.bmiHeader.biWidth = screenWidth;
        bmpInfo.bmiHeader.biHeight = -screenHeight; // Top-down bitmap
        bmpInfo.bmiHeader.biPlanes = 1;
        bmpInfo.bmiHeader.biBitCount = 32; // BGRA format
        bmpInfo.bmiHeader.biCompression = BI_RGB;

        cv::Mat screenMat(screenHeight, screenWidth, CV_8UC4); // BGRA format

        if (!GetDIBits(hdcMemory, hBitmap, 0, screenHeight, screenMat.data, &bmpInfo, DIB_RGB_COLORS)) {
            DeleteObject(hBitmap);
            DeleteDC(hdcMemory);
            ReleaseDC(NULL, hdcScreen);
            throw std::runtime_error("Failed to capture screen: GetDIBits failed.");
        }

        DeleteObject(hBitmap);
        DeleteDC(hdcMemory);
        ReleaseDC(NULL, hdcScreen);

        // Convert BGRA to BGR for OpenCV compatibility
        cv::cvtColor(screenMat, screenMat, cv::COLOR_BGRA2BGR);

        return screenMat;
    }

    void startDrawing(const std::string& imagePath, AutoClicker ac, int brush_size = 4) {
        // Initialize GDI+
        Gdiplus::GdiplusStartupInput gdiplusStartupInput;
        ULONG_PTR gdiplusToken;
        Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

        // Load the image
        std::wstring wImagePath(imagePath.begin(), imagePath.end());
        Gdiplus::Bitmap bitmap(wImagePath.c_str());

        if (bitmap.GetLastStatus() != Gdiplus::Ok) {
            throw std::runtime_error("Failed to load the image.");
        }

        // Get image dimensions
        int width = bitmap.GetWidth();
        int height = bitmap.GetHeight();

        // Process each pixel
        for (int y = 0; y < height; ++y) {
            int counter = 0;
            for (int x = 0; x < width; ++x) {
                Gdiplus::Color color;
                bitmap.GetPixel(x, y, &color);

                // Check if the pixel is white
                bool isWhite = (color.GetR() == 255 && color.GetG() == 255 && color.GetB() == 255);
                if (!isWhite) {
                    counter++;
                    if (counter == brush_size) {
                        ac.emulateMouseClick(200 + x, 300 + y);
                        counter = 0;
                    }
                }
                    
                //outputFile << (isWhite ? "0" : "1");
            }
            //outputFile << "\n";
        }


        // Cleanup
        Gdiplus::GdiplusShutdown(gdiplusToken);
    }

    void scaleBMP(const std::string& inputPath, const std::string& outputPath, int newWidth, int newHeight) {
        // Open the input BMP file
        std::ifstream inputFile(inputPath, std::ios::binary);
        if (!inputFile) {
            throw std::runtime_error("Failed to open input BMP file.");
        }

        // Read BMP file header
        BMPFileHeader fileHeader;
        inputFile.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
        if (fileHeader.bfType != 0x4D42) { // 'BM' in little-endian
            throw std::runtime_error("Invalid BMP file format.");
        }

        // Read BMP info header
        BMPInfoHeader infoHeader;
        inputFile.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));
        if (infoHeader.biBitCount != 24) { // Ensure 24-bit BMP
            throw std::runtime_error("Only 24-bit BMP files are supported.");
        }

        // Read the pixel data
        int originalWidth = infoHeader.biWidth;
        int originalHeight = infoHeader.biHeight;
        int originalRowSize = (originalWidth * 3 + 3) & ~3; // Align to 4 bytes
        std::vector<uint8_t> originalPixels(originalRowSize * std::abs(originalHeight));
        inputFile.seekg(fileHeader.bfOffBits, std::ios::beg);
        inputFile.read(reinterpret_cast<char*>(originalPixels.data()), originalPixels.size());
        inputFile.close();

        // Prepare scaled image data
        int scaledRowSize = (newWidth * 3 + 3) & ~3;
        std::vector<uint8_t> scaledPixels(scaledRowSize * newHeight);

        // Nearest neighbor scaling algorithm
        for (int y = 0; y < newHeight; ++y) {
            for (int x = 0; x < newWidth; ++x) {
                // Map scaled pixel to original pixel
                int originalX = x * originalWidth / newWidth;
                int originalY = y * originalHeight / newHeight;

                // Copy pixel data
                const uint8_t* originalPixel = &originalPixels[(originalY * originalRowSize) + (originalX * 3)];
                uint8_t* scaledPixel = &scaledPixels[(y * scaledRowSize) + (x * 3)];
                scaledPixel[0] = originalPixel[0]; // Blue
                scaledPixel[1] = originalPixel[1]; // Green
                scaledPixel[2] = originalPixel[2]; // Red
            }
        }

        // Update BMP info header for scaled image
        infoHeader.biWidth = newWidth;
        infoHeader.biHeight = newHeight;
        infoHeader.biSizeImage = scaledRowSize * newHeight;
        fileHeader.bfSize = sizeof(fileHeader) + sizeof(infoHeader) + infoHeader.biSizeImage;

        // Write the scaled BMP to the output file
        std::ofstream outputFile(outputPath, std::ios::binary);
        if (!outputFile) {
            throw std::runtime_error("Failed to open output BMP file.");
        }
        outputFile.write(reinterpret_cast<const char*>(&fileHeader), sizeof(fileHeader));
        outputFile.write(reinterpret_cast<const char*>(&infoHeader), sizeof(infoHeader));
        outputFile.write(reinterpret_cast<const char*>(scaledPixels.data()), scaledPixels.size());
        outputFile.close();
    }

    void reduceImageColors(const std::string& inputPath, const std::string& outputPath, const std::vector<ColorEntry>& colorPalette) {
        // Open the input BMP file
        std::ifstream inputFile(inputPath, std::ios::binary);
        if (!inputFile) {
            throw std::runtime_error("Failed to open input BMP file.");
        }

        // Read BMP file header
        BMPFileHeader fileHeader;
        inputFile.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
        if (fileHeader.bfType != 0x4D42) { // 'BM' in little-endian
            throw std::runtime_error("Invalid BMP file format.");
        }

        // Read BMP info header
        BMPInfoHeader infoHeader;
        inputFile.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));
        if (infoHeader.biBitCount != 24) { // Ensure 24-bit BMP
            throw std::runtime_error("Only 24-bit BMP files are supported.");
        }

        // Read the pixel data
        int width = infoHeader.biWidth;
        int height = infoHeader.biHeight;
        int rowSize = (width * 3 + 3) & ~3; // Align to 4 bytes
        std::vector<uint8_t> pixels(rowSize * std::abs(height));
        inputFile.seekg(fileHeader.bfOffBits, std::ios::beg);
        inputFile.read(reinterpret_cast<char*>(pixels.data()), pixels.size());
        inputFile.close();

        // Process each pixel
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                uint8_t* pixel = &pixels[y * rowSize + x * 3];
                RGB currentPixel = { pixel[2], pixel[1], pixel[0] }; // RGB from BGR
                RGB newColor = findClosestColor(currentPixel, colorPalette);

                // Update pixel with the closest color
                pixel[2] = newColor.r;
                pixel[1] = newColor.g;
                pixel[0] = newColor.b;
            }
        }

        // Write the new BMP file
        std::ofstream outputFile(outputPath, std::ios::binary);
        if (!outputFile) {
            throw std::runtime_error("Failed to open output BMP file.");
        }
        outputFile.write(reinterpret_cast<const char*>(&fileHeader), sizeof(fileHeader));
        outputFile.write(reinterpret_cast<const char*>(&infoHeader), sizeof(infoHeader));
        outputFile.write(reinterpret_cast<const char*>(pixels.data()), pixels.size());
        outputFile.close();
    }

    bool saveAsBMP(const std::string& filename, unsigned char* imageData, int width, int height) {
        // Swap RGB to BGR
        swapRGBToBGR(imageData, width, height);

        // Prepare the BMP headers
        BMPFileHeader fileHeader;
        BMPInfoHeader infoHeader;

        // BMP file header
        fileHeader.bfType = 0x4D42;  // 'BM'
        fileHeader.bfSize = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + width * height * 3;
        fileHeader.bfReserved1 = 0;
        fileHeader.bfReserved2 = 0;
        fileHeader.bfOffBits = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);

        // BMP DIB header
        infoHeader.biSize = sizeof(BMPInfoHeader);
        infoHeader.biWidth = width;
        infoHeader.biHeight = height;
        infoHeader.biPlanes = 1;
        infoHeader.biBitCount = 24;  // 24 bits per pixel for RGB
        infoHeader.biCompression = 0;  // No compression
        infoHeader.biSizeImage = width * height * 3;  // RGB
        infoHeader.biXPelsPerMeter = 2835;  // 72 DPI
        infoHeader.biYPelsPerMeter = 2835;  // 72 DPI
        infoHeader.biClrUsed = 0;
        infoHeader.biClrImportant = 0;

        // Open the output BMP file
        std::ofstream outFile(filename, std::ios::binary);
        if (!outFile) {
            std::cerr << "Error opening file for writing: " << filename << std::endl;
            return false;
        }

        // Write the file header
        outFile.write(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));

        // Write the DIB header
        outFile.write(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));

        // Write the pixel data (BMP stores data from bottom to top)
        int rowSize = (width * 3 + 3) & ~3;  // Row size must be a multiple of 4 bytes
        int paddingSize = rowSize - (width * 3);

        for (int y = height - 1; y >= 0; --y) {
            outFile.write(reinterpret_cast<const char*>(&imageData[y * width * 3]), width * 3);

            // Add padding if necessary
            if (paddingSize > 0) {
                std::vector<uint8_t> padding(paddingSize, 0);
                outFile.write(reinterpret_cast<const char*>(padding.data()), paddingSize);
            }
        }

        outFile.close();
        std::cout << "Image successfully saved as BMP to: " << filename << std::endl;
        return true;
    }

    // Function to locate the image on the screen
    cv::Point locateImageOnScreen(const cv::Mat& screenImage, const cv::Mat& templateImage) {
        if (screenImage.empty() || templateImage.empty()) {
            throw std::invalid_argument("Screen or template image is empty.");
        }

        if (templateImage.rows > screenImage.rows || templateImage.cols > screenImage.cols) {
            throw std::invalid_argument("Template image must be smaller than the screen image.");
        }

        cv::Mat result;
        int result_cols = screenImage.cols - templateImage.cols + 1;
        int result_rows = screenImage.rows - templateImage.rows + 1;
        result.create(result_rows, result_cols, CV_32FC1);

        cv::matchTemplate(screenImage, templateImage, result, cv::TM_CCOEFF_NORMED);

        double minVal, maxVal;
        cv::Point minLoc, maxLoc;
        cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

        if (maxVal < 0.8) { // Threshold to filter weak matches
            throw std::runtime_error("Template match confidence too low.");
        }

        return maxLoc;
    }
    
};

bool AutoClicker::testColorPalleteAlignment(std::vector<ColorEntry> colorPallete, ImageProcessor& ip, int cellWidth, int cellHeight, int bias) {
    // // Load the template image
    cv::Mat templateImage = cv::imread("pallete.png");

    if (templateImage.empty()) {
        std::cerr << "Error: Could not load the template image." << std::endl;
        return false;
    }

    try {
        cv::Mat screen = ip.captureScreen();
        cv::Point location = ip.locateImageOnScreen(screen, templateImage);
        std::cout << "Pallete found at: (" << location.x << ", " << location.y << ")" << std::endl;

        for (ColorEntry& ce : colorPallete) {
            
                emulateMouseClick(location.x + bias + (cellWidth * ce.y), location.y + bias + (cellHeight * ce.x));

                // Fetch the pixel color under the cursor
                COLORREF color = getPixelColorUnderCursor();

                // Extract the RGB components
                RGB currentColor = { GetRValue(color), GetGValue(color), GetBValue(color) };

                if (currentColor == ce.color) {
                    std::cout << "Found Color {" << int(currentColor.r) << "," << int(currentColor.g) << "," << int(currentColor.b) << "} at Position: (" << ce.x << "," << ce.y << ") with bias " << bias << std::endl;
                }
                else {
                    std::cerr << "COLOR NOT FIND color at Position: (" << ce.x << "," << ce.y << ") with bias " << bias << std::endl;
                    return false;
                }


            
        }

        std::cout << "\n[SUCCESS] All Colors in Pallete Found and are correctly aligned." << std::endl;
        std::cout << "You may continue using the AutoDraw Bot now" << std::endl;
        return true;

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << "\nColor Pallete for MS Paint NOT FOUND or Modified" << std::endl;
        std::cout << "Here\'s what you can do: " << std::endl;
        std::cout << "0) Ensure your display is not scaled above 100%" << std::endl;
        std::cout << "1) Try manually updating the bias value" << std::endl;
        std::cout << "2) Ensure the color pallete found in code is same as yours in MS Paint" << std::endl;
        std::cout << "3) Contact Developer" << std::endl;
        return false;
    }
}

void AutoClicker::selectColorFromPallete(std::vector<ColorEntry> colorPallete, RGB colorToSelect, ImageProcessor& ip, int cellWidth, int cellHeight, int bias) {
    // // Load the template image
    cv::Mat templateImage = cv::imread("pallete.png");

    if (templateImage.empty()) {
        std::cerr << "Error: Could not load the template image." << std::endl;
        return;
    }

    try {
        cv::Mat screen = ip.captureScreen();
        cv::Point location = ip.locateImageOnScreen(screen, templateImage);
        std::cout << "Pallete found at: (" << location.x << ", " << location.y << ")" << std::endl;

        for (ColorEntry &ce: colorPallete) {
            if (colorToSelect == ce.color) {
                emulateMouseClick(location.x + bias + (cellWidth * ce.y), location.y + bias + (cellHeight * ce.x));


            }
        }

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return;
    }

}


int main() {
    // Example usage: click at (500, 300)
    //int x;
    //int y;

    // Call the function to click at the specified location
    AutoClicker ac;
    //ac.emulateMouseClick(x, y);

    try {
        // Example usage
        std::string inputPath = "input.bmp";
        std::string outputPath = "output.txt";
        int newWidth = 500; // New width
        int newHeight = 352; // New height
        // Define the color palette (e.g., red, green, blue, white, black)


        std::vector<ColorEntry> colorPalette = {
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


        ImageProcessor ip;
        ImageDownloader id;

        // Select a color
        ac.selectColorFromPallete(colorPalette, { 112,146,190 }, ip, 22, 22, 10);
        //ac.testColorPalleteAlignment(colorPalette, ip, 22, 22, 10);
        // Download image as BMP
        //id.downloadImageUsingGoogleSearch("samsung", "download.bmp");
        

        // convert image into bmp
        int width, height, channels;

        // Load the image using stb_image
        unsigned char* imageData = stbi_load("download.bmp", &width, &height, &channels, STBI_rgb);

        if (imageData == nullptr) {
            std::cerr << "Error loading image: " << "download.bmp" << std::endl;
            return -1;
        }

        // Save the loaded image as input.bmp
        ip.saveAsBMP("input.bmp", imageData, width, height);

        // Free the image data after use
        stbi_image_free(imageData);

        ip.scaleBMP(inputPath, "image_scaled.bmp", newWidth, newHeight); // image is scaled
        std::cout << "Image scaled successfully. Saved to image_scaled.bmp" << std::endl;
        ip.reduceImageColors("image_scaled.bmp", "image_20bit.bmp", colorPalette); // Reduce the scaled image into a 20-bit color image
        //ip.startDrawing("image_20bit.bmp", ac);
        std::cout << "Image processed successfully. Check the output file: " << outputPath << std::endl;







        // // Load the template image
        //cv::Mat templateImage = cv::imread("pallete.png");

        //if (templateImage.empty()) {
        //    std::cerr << "Error: Could not load the template image." << std::endl;
        //    return -1;
        //}

        //try {
        //    while (true) {
        //        cv::Mat screen = ip.captureScreen();
        //        try {
        //            cv::Point location = ip.locateImageOnScreen(screen, templateImage);
        //            std::cout << "Template found at: (" << location.x << ", " << location.y << ")" << std::endl;
        //        }
        //        catch (const std::exception& e) {
        //            std::cerr << "Template not found: " << e.what() << std::endl;
        //        }

        //        // Optional: Add a delay between iterations to reduce CPU usage
        //        Sleep(1000); // 1 second delay
        //    }
        //}
        //catch (const std::exception& e) {
        //    std::cerr << "Error: " << e.what() << std::endl;
        //    return -1;
        //}


    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }

    return 0;
}
