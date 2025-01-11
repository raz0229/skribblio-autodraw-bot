#include "AutoClicker.h"
#include "ImageProcessor.h"
#include "config.h"
#include <windows.h>
#include <vector>
#include <tchar.h> // For _T macros

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
    cv::Mat templateImage = cv::imread(palleteImagePath);

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
                std::cerr << "COLOR NOT FOUND color at Position: (" << ce.x << "," << ce.y << ") with bias " << bias << std::endl;
                return false;
            }



        }

        std::cout << "\n[SUCCESS] All Colors in Pallete Found and are correctly aligned." << std::endl;
        std::cout << "You may continue using the AutoDraw Bot now" << std::endl;
        return true;

    }
    catch (const std::exception& e) {
        return false;
    }
}

void AutoClicker::selectColorFromPallete(std::vector<ColorEntry> colorPallete, RGB colorToSelect, ImageProcessor& ip, int cellWidth, int cellHeight, int bias) {
    // // Load the template image
    cv::Mat templateImage = cv::imread(palleteImagePath);

    if (templateImage.empty()) {
        std::cerr << "Error: Could not load the template image: " << palleteImagePath << std::endl;
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
                    // Retry (CAUTION: Can cause stack overflow)
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

POINT AutoClicker::getMouseClickCoords() {
    POINT point;

    // Infinite loop to check for mouse click
    while (true) {
        // Check if the left mouse button is clicked
        if (GetAsyncKeyState(VK_LBUTTON)) {
            // Get the current cursor position
            GetCursorPos(&point);
            return point; // Exit the function and return the coordinates
        }

        // Small sleep to avoid high CPU usage
        Sleep(10);
    }
}


void AutoClicker::openAndMaximizePaint() {
    // Find the Paint window
    HWND hwndPaint = FindWindow(_T("MSPaintApp"), NULL);
    if (hwndPaint == NULL) {
        MessageBox(NULL, _T("Could not find Microsoft Paint. Please launch it!"), _T("Error"), MB_ICONERROR);
        std::cout << "Launch the Paint app on Windows to proceed" << std::endl;
        system("pause");
        openAndMaximizePaint(); // Launch and maximize Paint if it's not running
        return;
    }

    // Restore the window if it's minimized
    if (IsIconic(hwndPaint)) {
        ShowWindow(hwndPaint, SW_RESTORE);
    }

    // Bring the window to the foreground
    SetForegroundWindow(hwndPaint);

    // Maximize the window
    ShowWindow(hwndPaint, SW_MAXIMIZE);
}

// Function to focus on the current console window
void AutoClicker::FocusConsoleWindow() {
    // Get the handle to the current console window
    HWND consoleWindow = GetConsoleWindow();

    if (consoleWindow == nullptr) {
        std::cerr << "Error: Could not retrieve the console window handle.\n";
        return;
    }

    // Bring the console window to the foreground
    if (SetForegroundWindow(consoleWindow)) {
        std::cout << "Console window brought to the foreground successfully.\n";
    }
    else {
        std::cerr << "Error: Failed to bring the console window to the foreground.\n";
    }
}



