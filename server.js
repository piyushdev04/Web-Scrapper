const express = require('express');
const { exec } = require('child_process');
const path = require('path');

const app = express();
app.use(express.json());
app.use(express.static('public'));

app.post('/scrape', (req, res) => {
    const urls = req.body.urls;
    const input = urls.join('\n') + '\ndone\n';

    const process = exec('./scraper', (err, stdout, stderr) => {
        if (err) return res.status(500).send(stderr || err.message);
        res.sendFile(path.join(__dirname, 'scraped_links.csv'));
    });

    process.stdin.write(input);
    process.stdin.end();
});

app.listen(3000, () => {
    console.log('Server running at http://localhost:3000');
});