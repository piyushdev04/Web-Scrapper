#include<iostream> // standard input/output
#include<curl/curl.h> // HTTP requests via cURL
#include<regex> // Regular expressions for extracting links
#include<fstream> // file handling (to save extracting links)
#include<thread> // Multi-threading support
#include<vector> // Dynamic array for storing URLs
#include<mutex> // Prevents race conditions in multi-threading

using namespace std;

mutex mtx; // Mutex for thread-safe output

// Callback function to store the fetched data
size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* userData) {
    size_t totalSize = size * nmemb;
    userData->append((char*)contents, totalSize);
    return totalSize;
}

// Function to fetch webpage HTML
string fetchHTML(const string& url) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        cerr << "Failed to initialized cURL!" << endl;
        return "";
    }

    string htmlContent;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &htmlContent);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // Follow redirects
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L); // Timeout after 10 sec

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        cerr << "cURL Error: " << curl_easy_strerror(res) << endl;
        return "";
    }

    curl_easy_cleanup(curl);
    return htmlContent;
}

// Function to extract links and save to a file
void extractAndSaveLinks(const string& html, const string& url, ofstream& outFile) {
    regex linkRegex(R"(<a\s+[^>]*href=["']([^"']+)["'])");
    smatch match;
    string::const_iterator searchStart(html.cbegin());

    vector<string> links;
    while (regex_search(searchStart, html.cend(), match, linkRegex)) {
        links.push_back(match[1]);
        searchStart = match.suffix().first;
    }

    // Lock mutex to prevent race conditions while writing
    lock_guard<mutex> lock(mtx);
    for (const auto& link : links) {
        outFile << url << "," << link << "\n";
    }
}

// Function for multi-threaded web scrapping
void scrapeWebsite(const string& url, ofstream& outFile) {
    cout << "Fetching: " << url << "...\n";
    string html = fetchHTML(url);
    if (!html.empty()) {
        extractAndSaveLinks(html, url, outFile);
    } else {
        cerr << "Failed to fetch: " << url << endl;
    }
}

int main() {
    vector<string> urls;
    int numThreads = 2;

    cout << "Enter website URLs (type 'done' to finish):\n";
    while (true) {
        string url;
        cin >> url;
        if (url == "done") break;
        urls.push_back(url);
    }

    ofstream outFile("scraped_links.csv");
    if (!outFile) {
        cerr << "Error opening file for writing!" << endl;
        return 1;
    }
    outFile << "Source URL,Extracted Link\n";

    vector<thread> threads;
    for (const auto& url : urls) {
        if (threads.size() >= numThreads) {
            for (auto& th : threads) th.join(); // Wait for threads to finish
            threads.clear();
        }
        threads.emplace_back(scrapeWebsite, url, ref(outFile));
    }

    for (auto& th : threads) th.join(); // Ensure all threads complete

    outFile.close();
    cout << "Scraping completed. Results saved to scraped_links.csv\n";

    return 0;
}