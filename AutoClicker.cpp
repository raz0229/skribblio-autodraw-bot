#include "AutoClicker.h"
#include "ImageProcessor.h"
#include <windows.h>
#include <vector>

COLORREF AutoClicker::getPixelColorUnderCursor() {
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


// Function to emulate a mouse click at given x, y coordinates
void AutoClicker::emulateMouseClick(int x, int y, bool addDelay) {
    // Move the cursor to the specified coordinates
    SetCursorPos(x, y);

    if (addDelay) {
        /*
            A slight dynamic Delay is recommended especially when selecting a color,
            the correct execution of which is absolutely necessary for the bot to draw right
        */

        // Get the current tick count
        DWORD start_time = GetTickCount64();

        // Wait for a short duration (e.g., 50 milliseconds)
        while (GetTickCount64() - start_time < 50);
    }

    // Emulate mouse left button down and up
    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}

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

        bool found = false;
        for (ColorEntry& ce : colorPallete) {
            if (colorToSelect == ce.color) {
                emulateMouseClick(location.x + bias + (cellWidth * ce.y), location.y + bias + (cellHeight * ce.x), true);



                // Fetch the pixel color under the cursor
                COLORREF color = getPixelColorUnderCursor();

                // Extract the RGB components
                RGB currentColor = { GetRValue(color), GetGValue(color), GetBValue(color) };

                if (currentColor == ce.color) {
                    std::cout << "Found Color {" << int(currentColor.r) << "," << int(currentColor.g) << "," << int(currentColor.b) << "} at Position: (" << ce.x << "," << ce.y << ") with bias " << bias << std::endl;
                }
                else {
                    std::cerr << "COLOR NOT FOUND at Position: (" << ce.x << "," << ce.y << ") with bias " << bias << std::endl;
                    std::cerr << "click x = " << (location.x + bias + (cellWidth * ce.y)) << " click y = " << (location.y + bias + (cellHeight * ce.x)) << std::endl;
                    return;
                    // Retry
                    //selectColorFromPallete(colorPallete, ce.color, ip, cellWidth, cellHeight, bias);
                }


                std::cout << "=== COLOR FOUND (" << (int)colorToSelect.r << "," << (int)colorToSelect.g << "," << (int)colorToSelect.b << ")===\n";
                found = true;

            }
        }

        if (!found)
            std::cout << "=== COLOR NOT FOUND (" << (int)colorToSelect.r << "," << (int)colorToSelect.g << "," << (int)colorToSelect.b << ")===\n";

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return;
    }

}
