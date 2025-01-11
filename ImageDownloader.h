#pragma once
#ifndef IMAGEDOWNLOADER_H
#define IMAGEDOWNLOADER_H


#include <windows.h>
#include <fstream>
#include <iostream>
#include <wininet.h>

class ImageDownloader {

    const std::string& apiKey;
    std::string searchSuffix;

    bool downloadData(const std::string& url, std::string& response);
    std::string extractImageUrl(const std::string& json);
    bool downloadImage(const std::string& url, const std::string& outputFile);

public:
    ImageDownloader(const std::string& key, std::string suffix) :apiKey(key) ,searchSuffix(suffix) {}
    void downloadImageUsingGoogleSearch(const std::string& searchTerm, const std::string& outputFile);
};


#endif
