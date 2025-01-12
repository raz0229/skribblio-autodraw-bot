/*
    https://www.geeksforgeeks.org/opencv-c-windows-setup-using-visual-studio-2019/
*/
#define STB_IMAGE_IMPLEMENTATION
#include "third-party/dotenv.h"
#include "config.h"
#include "AutoClicker.h"
#include "ImageProcessor.h"
#include "ImageDownloader.h"


// include gdiplus as linked library
#pragma comment(lib, "gdiplus.lib")


int main() {

    dotenv env(".env");

    // Access environment variables
    std::string x_apikey = env.get("GOOGLE_CUSTOM_SEARCH_API_KEY", "no-key");
    std::string bias_s = env.get("BIAS", biasCustomValue);
    int bias = std::atoi(biasCustomValue.c_str());


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
    ImageDownloader id(x_apikey, searchTermSuffix);

    try {

        std::cout << "[INFO] Preparing for first time run..." << std::endl;
        std::cout << "[WARNING] Leave your Mouse and Keyboard for a few seconds" << std::endl;
        AutoClicker::openAndMaximizePaint();
       
        // Test color Pallete Alignment
        Sleep(1000);
        if (!ac.testColorPalleteAlignment(colorPalette, ip, cellWidthMSPaint, cellHeightMSPaint, bias)) {
            std::cerr << "\nColor Pallete for MS Paint NOT FOUND or Modified" << std::endl;
            std::cout << "Here\'s what you can do: " << std::endl;
            std::cout << "0) Ensure your system display is not scaled above 100%" << std::endl;
            std::cout << "1) Try manually updating the bias value in .env file" << std::endl;
            std::cout << "2) Ensure the color pallete found in code: \'" << palleteImagePath << "\' is same as yours in MS Paint" << std::endl;
            std::cout << "3) Contact Developer: @raz0229" << std::endl;
            return -1;
        }

        
        while (1) {
            // Download image as BMP
            AutoClicker::FocusConsoleWindow();
            system("cls");
            std::string searchTerm;
            std::cout << "Enter what to draw: ";
            getline(std::cin, searchTerm);
            
            if (!id.downloadImageUsingGoogleSearch(searchTerm, imageDownloadPath)) {
                std::cout << "[ERROR] You need a stable internet connection to proceed." << std::endl;
                system("pause");
                return -1;
            }


            // Convert image into bmp
            int width, height, channels;
            unsigned char* imageData = stbi_load(imageDownloadPath.c_str(), &width, &height, &channels, STBI_rgb);        // Load the image using stb_image
            if (imageData == nullptr) {
                std::cerr << "Error loading image: " << imageDownloadPath << std::endl;
                return -1;
            }
            ip.saveAsBMP(inputPath, imageData, width, height);  // Save the loaded image
            stbi_image_free(imageData);         // Free the image data after use


            // Scale the converted image
            ip.scaleBMP(inputPath, scaledImagePath, newWidth, newHeight);
            std::cout << "Image scaled successfully. Saved to: " << scaledImagePath << std::endl;
            ip.reduceImageColors(scaledImagePath, finalProcessedImagePath, colorPalette); // Reduce the scaled image into a 20-bit color image
            std::cout << "Image processed successfully." << std::endl;
            system("cls");

            // Select a starting position
            std::cout << "[INFO] Your image is ready" << std::endl;
            std::cout << "[INFO] You\'ll now SWITCH to Paint Window automatically (after pressing any key)" << std::endl;
            std::cout << "[INFO] Select any Brush of your choice in MS Paint and set its size to 5px" << std::endl;
            std::cout << "[INFO] CLICK ANYWHERE in the PAINT window to start drawing" << std::endl;
            system("pause");
            AutoClicker::openAndMaximizePaint();
            POINT clickPosition = AutoClicker::getMouseClickCoords();

            // Split image colors and start drawing
            ip.splitImageByColorsAndStartDrawing(finalProcessedImagePath, ac, colorPalette, clickPosition, cellWidthMSPaint, cellHeightMSPaint, bias);
            std::cout << "[INFO] Finished Drawing! Hope you like it :)" << std::endl;
        }




        


    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }

    return 0;
}
