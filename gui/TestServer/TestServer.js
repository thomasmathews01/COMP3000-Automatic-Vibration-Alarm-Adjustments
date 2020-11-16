const express = require('express');
const cors = require('cors');
const {json, urlencoded} = require("body-parser");

const port = 3456;

const app = express();
app.use(cors());
app.use(json());
app.use(urlencoded({extended: true}));

app.get('/', (req, res) => {
    res.send('Hello World!')
});

app.post('/login', (req, res) => {
    const username = req.query.username;
    const password = req.query.password;

    if (username !== undefined && password !== undefined) {
        if (username === "admin" && password === "admin") {
            res.status(200);
            res.send("Accepted");
        }
        res.status(404);
        res.send("Rejected");
    }

    res.status(400);
});

app.get('/sites', (req, res) => {
    const info = {
        sites: [
            {
                id: 1,
                name: "Surrey"
            },
            {
                id: 2,
                name: "Devon"
            },
            {
                id: 3,
                name: "Scotland"
            },
            {
                id: 4,
                name: "Heaven"
            },
            {
                id: 5,
                name: "Northanger"
            },
            {
                id: 6,
                name: "Highbury"
            },
        ]
    };

    res.send(JSON.stringify(info));
});

app.get('/machines', (req, res) => {
    const site1Machines = {
        machines: [
            {
                id: 1,
                name: "TG 7"
            },
            {
                id: 2,
                name: "TG 8"
            },
            {
                id: 3,
                name: "PT 1"
            },
            {
                id: 4,
                name: "PT 2"
            },
        ]
    };

    if (req.query.site !== undefined) {
        res.status(200);
        res.send(JSON.stringify(site1Machines));
    }

    res.status(400); // TODO: The API needs a much better error response than this
});

app.get('/channels', (req, res) => {
    const channelInformation = [
        { // Machine TG 7
            id: 1,
            name: "Channel 1",
            units: "mm/s"
        },
        { // Machine TG 7
            id: 2,
            name: "Channel 2",
            units: "mm/s"
        },
        { // Machine TG 8
            id: 3,
            name: "Channel 3",
            units: "mm/s"
        },
        { // Machine TG 8
            id: 4,
            name: "Channel 4",
            units: "mm/s"
        },
        { // Machine PT 1
            id: 5,
            name: "Channel 5",
            units: "mm/s"
        },
        { // Machine PT 1
            id: 6,
            name: "Channel 6",
            units: "mm/s"
        },
        { // Machine PT 2
            id: 7,
            name: "Channel 7",
            units: "mm/s"
        },
        { // Machine PT 2
            id: 8,
            name: "Channel 8",
            units: "mm/s"
        }
    ];

    if (req.query.machine !== undefined) {
        if (req.query.machine > 0 && req.query.machine <= 4) {
            res.send(JSON.stringify({channels: channelInformation}));
        }
    }
});

app.get('/', (req, res) => {
    res.send('Hello World!')
});

app.listen(port, () => {
    console.log(`Example app listening at http://localhost:${port}`)
});