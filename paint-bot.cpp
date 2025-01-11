/*
    https://www.geeksforgeeks.org/opencv-c-windows-setup-using-visual-studio-2019/
*/
#define STB_IMAGE_IMPLEMENTATION
#include <windows.h>
#include <gdiplus.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdint>
#include <wininet.h>
#include <set>
#include <string>
#include <unordered_map>
#include <opencv2/opencv.hpp>
#include "bitmap_structs.h"
#include "AutoClicker.h"
#include "ImageProcessor.h"
#include "ImageDownloader.h"
#include "third-party/dotenv.h"



// include gdiplus as linked library
#pragma comment(lib, "gdiplus.lib")





int main() {

    dotenv env(".env");

    // Access environment variables
    std::string x_apikey = env.get("GOOGLE_CUSTOM_SEARCH_API_KEY", "no-key");
    std::string bias_s = env.get("BIAS", "15");
    int bias = 15;


    if (x_apikey == "no-key") {
        std::cerr << "No GOOGLE_CUSTOM_SEARCH_API_KEY found in .env : Program is exiting";
        return -1;
    }
    else {
        bias = std::atoi(bias_s.c_str());
        std::cout << "[LOADED] API_KEY = " << x_apikey << std::endl;
        std::cout << "[LOADED] BIAS = " << bias << std::endl << std::endl;
    }

    AutoClicker ac;
    ImageProcessor ip;
    ImageDownloader id(x_apikey, " white background");  // Add "white background" to every searh query

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

        // Test color Pallete Alignment
        //ac.testColorPalleteAlignment(colorPalette, ip, 22, 22, 10);
       
        /*Sleep(3000);
        for (auto& c : colorPalette) {
            ac.selectColorFromPallete(colorPalette, c.color, ip, 22, 22, 10);
            Sleep(1000);
        }

        system("pause");*/

       
        // Download image as BMP
        std::string searchTerm;
        std::cout << "Enter what to draw: ";
        getline(std::cin, searchTerm);
        id.downloadImageUsingGoogleSearch(searchTerm, "download.bmp");
        

        // convert image into bmp
        int width, height, channels;
        //Sleep(3000);
        //ac.emulateMouseClick(885, 61);
        //std::cout << "881, 61" << std::endl;
        //system("pause");

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
        std::cout << "Image processed successfully. Check the output file: " << outputPath << std::endl;


        // Split image colors and start drawing
        std::cout << "Sleeping for 3 seconds" << std::endl;
        Sleep(3000);
        ip.splitImageByColorsAndStartDrawing("image_20bit.bmp", ac, colorPalette, 22,22,bias);


        


    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }

    return 0;
}
