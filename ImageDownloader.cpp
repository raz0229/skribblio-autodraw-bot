#include "ImageDownloader.h"


// Helper function to download data via HTTP using the WinINet API
bool ImageDownloader::downloadData(const std::string& url, std::string& response) {
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
std::string ImageDownloader::extractImageUrl(const std::string& json) {
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
bool ImageDownloader::downloadImage(const std::string& url, const std::string& outputFile) {
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


    bool ImageDownloader::downloadImageUsingGoogleSearch(const std::string& searchTerm, const std::string& outputFile) {
        try {
            const std::string cx = "7204b6b1decb42058";
            const std::string query = searchTerm + searchSuffix;
            const std::string url = "https://www.googleapis.com/customsearch/v1?q=" + query +
                "&cx=" + cx + "&searchType=image&imgSize=LARGE&safe=high&key=" + apiKey;

            // Download the search result JSON
            std::string response;
            if (!downloadData(url, response)) {
                throw std::runtime_error("Failed to download JSON response from Google Custom Search API.");
            }

            // Extract the image URL from the JSON response
            std::string imageUrl = extractImageUrl(response);
            if (imageUrl.empty()) {
                throw std::runtime_error("Failed to extract image URL from JSON response.");
            }

            std::cout << "Image URL: " << imageUrl << std::endl;

            // Download the image and save it as BMP
            if (!downloadImage(imageUrl, outputFile)) {
                throw std::runtime_error("Failed to download and save the image.");
            }

            std::cout << "Image successfully saved to: " << outputFile << std::endl;
            return true;
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return false;
        }
    }