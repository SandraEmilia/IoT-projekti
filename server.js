const express = require('express');
const fs = require('fs');
const path = require('path');
const app = express();
const port = 8080;

app.use(express.static(path.join(__dirname)));
app.use(express.json());

// Hae lämpötila

app.get('/temperature', (req, res) => {
    try {
        const temperature = JSON.parse(fs.readFileSync('temperature.txt', 'utf-8'));
        res.json(temperature);
    } catch (err) {
        res.status(500).send('Error reading temperature');
    }
});

// Päivitä lämpötila

app.post('/temperature', (req, res) => {
    const {temperature} = req.body;
    if (temperature) {
        const temperature_to_update = { temperature: parseInt(temperature) };
    fs.writeFileSync('temperature.txt', JSON.stringify(temperature_to_update));
    res.send('Temperature updated!');    
    } else {
        res.status(400).send('Invalid parameters');
    }
});

// Päivitä asetukset

app.post('/settings', (req, res) => {
    const { minTemperature, maxTemperature } = req.body;
    if (minTemperature && maxTemperature) {
        const settings = {
            minTemperature: parseInt(minTemperature),
            maxTemperature: parseInt(maxTemperature)
        };
        fs.writeFileSync('settings.txt', JSON.stringify(settings));
        res.send('Settings updated!');
    } else {
        res.status(400).send('Invalid parameters');
    }
});

app.get('/settings', (req, res) => {
    try {
        const settings = JSON.parse(fs.readFileSync('settings.txt', 'utf-8'));
        res.json(settings);
    } catch (err) {
        res.status(500).send('Error reading settings');
    }
});

app.listen(port, () => {
    console.log(`Server running at http://localhost:${port}`);
});