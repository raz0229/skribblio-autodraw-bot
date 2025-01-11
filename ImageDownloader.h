#pragma once
#ifndef IMAGEDOWNLOADER_H
#define IMAGEDOWNLOADER_H


#include <windows.h>
#include <fstream>
#include <iostream>
#include <wininet.h>

class ImageDownloader {

    bool downloadData(const std::string& url, std::string& response);
    std::string extractImageUrl(const std::string& json);
    bool downloadImage(const std::string& url, const std::string& outputFile);

public:
    void downloadImageUsingGoogleSearch(const std::string& searchTerm, const std::string& outputFile);
};


#endif
