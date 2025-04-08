# Multithreaded Web Scraper

A simple multithreaded web scraper in C++ with frontend. Extracts all `<a href>` links from given URLs and saves them to a CSV file.

---

## Features

- Extracts hyperlinks from websites
- Saves results to `scraped_links.csv`
- Multithreaded for faster scraping
- Clean web interface (Node.js + Express backend)

---

## Web Scraper (`main.cpp`)

### Description

This C++ program performs the core scraping:

- Uses **libcurl** to fetch HTML content
- Applies **regex** to extract all hyperlinks
- Supports **multithreading** using `std::thread`
- Prevents race conditions using `std::mutex`
- Saves results to `scraped_links.csv`

### Main Components

- **WriteCallback**: Callback to collect HTML data
- **fetchHTML(url)**: Downloads HTML using libcurl
- **extractAndSaveLinks(html, url, outFile)**: Uses regex to extract all `<a href="">` values and write to file
- **scrapeWebsite(url, outFile)**: Manages fetching and extraction for each URL
- **main()**:
  - Reads URLs from `cin`
  - Launches threads to process each URL
  - Outputs CSV format: `Source URL, Extracted Link`

### Backend (server.js)
- Accepts URLs from the frontend via POST
- Launches the compiled C++ scraper via `child_process`
- Sends back the resulting CSV file

### Build Command

```bash
g++ main.cpp -o scraper -lcurl -pthread
```

### Run Command

```bash
node server.js
```