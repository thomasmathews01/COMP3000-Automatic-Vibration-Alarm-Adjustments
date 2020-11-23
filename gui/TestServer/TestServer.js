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

app.get('/dataTypes', (req, res) => {
    const typeInformation = [
        {id: 1, name: "SUBSYNC"},
        {id: 2, name: "SUBSYNC_FREQ"},
        {id: 3, name: "PK2PK"},
        {id: 4, name: "NONSYNC"},
        {id: 5, name: "RMS"},
        {id: 6, name: "ORDER1"},
        {id: 7, name: "ORDER2"},
        {id: 8, name: "ORDER3"},
        {id: 9, name: "ORDER4"},
        {id: 10, name: "INTRAHARMONIC"},
    ];

    if (req.query.channel !== undefined) {
        res.send(JSON.stringify({types: typeInformation}));
    }
});

app.get('/data', (req, res) => {
    const pointCount = 5000;
    const data = Array.from({length: pointCount}, (x, y) => Math.abs(200 * Math.sin((y / pointCount) * 5 * 2 * Math.PI)));
    const StartDate = new Date(2019, 11, 23, 10, 53, 0, 0);
    const startEpochSeconds = Math.round(StartDate.getTime() / 1000);
    const time = Array.from({length: pointCount}, (x, index) => (index * 3600) + startEpochSeconds);

    if (req.query.channel !== undefined && req.query.type !== undefined) {
        res.send(JSON.stringify({
            data: time.map((timestamp, index) => {
                return {secondsSinceEpoch: timestamp, value: data[index]}
            })
        }));
    }
});

app.get('/alarms', (req, res) => {
    const alarmData = [
        {secondsSinceEpoch: Math.round(new Date(2019, 11, 23, 10, 53, 0, 0).getTime() / 1000), value: 150},
        {secondsSinceEpoch: Math.round(new Date(2020, 1, 23, 10, 53, 0, 0).getTime() / 1000), value: 195}
    ];

    if (req.query.channel !== undefined && req.query.type !== undefined) {
        res.send(JSON.stringify({
            data: alarmData
        }));
    }
});

app.get('/alerts', (req, res) => {
    const alarmData = [
        {secondsSinceEpoch: Math.round(new Date(2019, 11, 23, 10, 53, 0, 0).getTime() / 1000), value: 130},
        {secondsSinceEpoch: Math.round(new Date(2020, 1, 23, 10, 53, 0, 0).getTime() / 1000), value: 170}
    ];

    if (req.query.channel !== undefined && req.query.type !== undefined) {
        res.send(JSON.stringify({
            data: alarmData
        }));
    }
});

app.get('/', (req, res) => {
    res.send('Hello World!')
});

app.listen(port, () => {
    console.log(`Example app listening at http://localhost:${port}`)
});