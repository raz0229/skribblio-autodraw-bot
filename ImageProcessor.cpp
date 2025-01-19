#include "ImageProcessor.h"

// Find the closest color in the palette
RGB ImageProcessor::findClosestColor(const RGB& pixel, const std::vector<ColorEntry>& colorPalette) {
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

// Swap RGB to BGR
void ImageProcessor::swapRGBToBGR(unsigned char* imageData, int width, int height) {
    for (int i = 0; i < width * height; ++i) {
        unsigned char* pixel = &imageData[i * 3];
        unsigned char temp = pixel[0];
        pixel[0] = pixel[2];
        pixel[2] = temp;
    }
}

// Capture the screen as a cv::Mat
cv::Mat ImageProcessor::captureScreen() {
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

// Start drawing on an image
void ImageProcessor::startDrawing(const cv::Mat& image, AutoClicker& ac, POINT clickPosition, int brush_size) {
    if (image.empty()) {
        throw std::runtime_error("The provided image is empty.");
    }

    int width = image.cols;
    int height = image.rows;

    for (int y = 0; y < height; ++y) {
        int counter = 0;
        for (int x = 0; x < width; ++x) {
            cv::Vec3b color = image.at<cv::Vec3b>(y, x);
            bool isWhite = (color[0] == 255 && color[1] == 255 && color[2] == 255);
            if (!isWhite) {
                counter++;
                if (counter == brush_size) {
                    ac.emulateMouseClick(clickPosition.x + x, clickPosition.y + y);
                    counter = 0;
                }
            }
        }
    }
}

// Scale a BMP image
void ImageProcessor::scaleBMP() {

    const std::string& inputPath = getInputPath(); 
    const std::string& outputPath = getScaledImagePath();
    int newWidth = getNewWidth();
    int newHeight = getNewHeight();

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

// Reduce the number of colors in a BMP image
void ImageProcessor::reduceImageColors(const std::vector<ColorEntry>& colorPalette) {
    const std::string& inputPath = getScaledImagePath();
    const std::string& outputPath = getFinalProcessedImagePath();

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

// Save an image as BMP
bool ImageProcessor::saveAsBMP(unsigned char* imageData, int width, int height) {
    // Swap RGB to BGR
    swapRGBToBGR(imageData, width, height);
    const std::string& filename = getInputPath();

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

// Custom hash function for cv::Vec3b
std::size_t ImageProcessor::Vec3bHash::operator()(const cv::Vec3b& color) const {
    return ((std::size_t)color[0]) | (((std::size_t)color[1]) << 8) | (((std::size_t)color[2]) << 16);
}

// Split the image by colors and start drawing
void ImageProcessor::splitImageByColorsAndStartDrawing(AutoClicker& ac, std::vector<ColorEntry> colorPallete, POINT clickPosition, int bias) {
    const std::string& inputFile = getFinalProcessedImagePath();
    int cellWidth = getCellWidthMSPaint();
    int cellHeight = getCellHeightMSPaint();


    // Load the BMP image
    cv::Mat image = cv::imread(inputFile, cv::IMREAD_COLOR);
    if (image.empty()) {
        std::cerr << "Error: Could not load the image." << std::endl;
        return;
    }

    // Map to store unique colors and their associated output Mat
    std::unordered_map<cv::Vec3b, cv::Mat, Vec3bHash> colorMaps;

    // Create a white background
    cv::Mat whiteBackground(image.size(), image.type(), cv::Scalar(255, 255, 255)); // Proper white background (BGR)

    // Iterate through every pixel in the image
    for (int row = 0; row < image.rows; ++row) {
        for (int col = 0; col < image.cols; ++col) {
            // Get the current pixel color
            cv::Vec3b color = image.at<cv::Vec3b>(row, col);

            // Check if the color is already in the map
            if (colorMaps.find(color) == colorMaps.end()) {
                // If not, create a new blank Mat and add it to the map
                colorMaps[color] = whiteBackground.clone();
            }

            // Set the pixel in the specific color map
            colorMaps[color].at<cv::Vec3b>(row, col) = color;
        }
    }

    int count = 0;
    for (const auto& pair : colorMaps) {
        const cv::Vec3b& color = pair.first;
        const cv::Mat& coloredImage = pair.second;

        // Print the RGB value of the color (convert from BGR to RGB)
        std::cout << "Selecting color " << ++count << " RGB("
            << (int)color[2] << ", "  // Red
            << (int)color[1] << ", "  // Green
            << (int)color[0] << ")"   // Blue
            << std::endl;

        // Select a color
        //Sleep(1000);
        Image img;
        ac.selectColorFromPallete(img, colorPallete, { color[2], color[1], color[0] }, *this, cellWidth, cellHeight, bias);
        this->startDrawing(coloredImage, ac, clickPosition);

    }
}

// Locate the image on the screen
cv::Point ImageProcessor::locateImageOnScreen(const cv::Mat& screenImage, const cv::Mat& templateImage) {
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
