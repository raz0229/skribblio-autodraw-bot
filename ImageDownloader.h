#pragma once
#ifndef IMAGEDOWNLOADER_H
#define IMAGEDOWNLOADER_H


#include "config.h"
#include <windows.h>
#include <fstream>
#include <iostream>
#include <wininet.h>

class ImageDownloader : public Image {
     
    const std::string& apiKey;
    std::string searchSuffix;

    bool downloadData(const std::string& url, std::string& response);
    std::string extractImageUrl(const std::string& json);
    bool downloadImage(const std::string& url, const std::string& outputFile);

public:
    ImageDownloader(const std::string& key) :apiKey(key) ,searchSuffix(getSearchTermSuffix()) {}
    bool downloadImageUsingGoogleSearch(const std::string& searchTerm);
};


#endif
