document.getElementById('scrape-form').addEventListener('submit', async (e) => { 
    e.preventDefault(); 

    const resultBox = document.getElementById('result');
    resultBox.innerHTML = "Scrapping in progress... Please wait.";

    const urls = document.getElementById('urls').value
        .trim()
        .split('\n')
        .map(url => url.trim())
        .filter(url => url);

        if (urls.lenght === 0) {
            resultBox.innerHTML = "Please enter at least one valid URL.";
            return;
        }

        try {
            const response = await fetch('/scrape', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify({ urls })
            });

            if (!response.ok) {
                const err = await response.text();
                throw new Error(err);
            }

            const blob = await response.blob();
            const downloadlink = document.createElement('a');
            downloadlink.href = URL.createObjectURL(blob);
            downloadlink.download = 'scraped_links.csv';
            downloadlink.click();

            resultBox.innerHTML = "Scrapping complete. CSV file downloaded!";
        } catch (err) {
            resultBox.innerHTML = `Error: ${err.message}`;
        }
});