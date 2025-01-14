# MS Paint - AutoDraw Bot
This project is a reimplementation of the same idea that we did in Python but now in CPP. It uses the Windows API and OpenCV mainly to download an image for the provided prompt from the web and then uses a defined palette (for MS Paint) in this case to draw the image on the screen.
## Group Members
**Abdullah Zafar _(L1F23BSAI0054)_**
Abdul-Rehman (L1F23BSAI0050)
Talha Abid (L1F23BSAI0058)

Submitted to: _**M Umar**_

## Code:
All the code for this project can be found on:
[this repository](https://github.com/raz0229/skribblio-autodraw-bot/blob/cpp/)
 _(Choose the `CPP` branch for this project)_

## Technical Details
The project is implemented using strong object orientation and coding techniques and style learnt in class. It uses Google's Custom Search API _(API Key can be added in .env file)_ to look for the image results and converts the image into a 20-bit (standard color palette supported by MS Paint) bitmap file _(.bmp)_ which is necessary to preserve the order of pixels in rows and columns to calculate their offset from a starting position. It uses the Windows API to handle Paint and Console Windows and adjust their focus. Likewise, OpenCV is used to look for the desired color pallete on Screen and be able to select all the colors in tray. The downloaded image is split into all the colors it had and each color is drawn on screen (by mouse click for each pixel) relative to the starting position selected by the user.

- ### System Requirements
	Windows 10/8.1/8/7 
	

> Windows 11 is not yet supported by the executable found in Release page of this repository due to a different MS Paint version shipped with it in newer version of Windows, but you're welcome to add the palette for it and test all the color selections for it in configuration if you want

- #### Dependencies
	Windows API
	OpenCV
	dot-env (for C++)
	stb_image (for bitmap conversion)

## Debug and Run
- ### Step 1:
	Clone the repository:
	[GitHub@raz0229/skribblio-autodraw-bot](https://github.com/raz0229/skribblio-autodraw-bot/tree/cpp
	
	_(Make sure the branch is `CPP`)_
- ### Step 2:
Download and setup OpenCV (for development)


This guide explains how to set up and install OpenCV for C++ development in Visual Studio 2019. By following these steps, you will be able to create robust applications using OpenCV for real-time computer vision, video capturing, image processing, and machine learning.

## Prerequisites
- Visual Studio 2019 installed
- A C++ development environment set up

## Steps to Install and Set Up OpenCV

### 1. Download and Install OpenCV
1. Go to the [OpenCV Releases page](https://opencv.org/releases/).
2. Select the latest version (e.g., 4.5.0) and click the **Windows** button to download the setup file.
3. Open the downloaded file. A self-extracting zip will appear.
4. Create a folder named `OpenCV` inside your `C:\` drive.
5. Extract the contents of the zip file to the folder `C:\opencv`.

### 2. Add OpenCV Binaries to System Path
1. Open a Command Prompt with **admin privileges**.
2. Run the following command to add OpenCV as a system variable:
   ```bash
   setx -m OPENCV_DIR C:\opencv\build\x64\vc15


> For Release, add `opencv_world4100.lib`into your Linker -> Input -> Additional Dependencies

## Installation and Run
Installtion of the bot is pretty straightforward. There is a guided installer made using InnoInstaller and it creates a Desktop Shortcut to run the bot and guides you through the proper steps to launch and use it.

## Configuration
In the directory where you chose to install the program, there is an `.env` file:

    GOOGLE_CUSTOM_SEARCH_API_KEY=""
    BIAS=14

Replace the API Key with your own _(if not already present)_
You may tweak the BIAS value according to your own system if some colors are not being selected by the bot.
