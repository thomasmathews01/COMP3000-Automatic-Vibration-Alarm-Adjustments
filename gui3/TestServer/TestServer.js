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
                name: "Highbury"
            },
            {
                id: 2,
                name: "Northanger"
            },
            {
                id: 3,
                name: "Surrey"
            },
            {
                id: 4,
                name: "Devon"
            },
            {
                id: 5,
                name: "Heaven"
            },
            {
                id: 6,
                name: "Scotland"
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

app.get('/data', async (req, res) => {
    console.log("Data Requested!");
    if (!req.query.start || !req.query.end || !req.query.channel || !req.query.type) {
        console.log("Data Requested badly!");
        res.status(400); // Forgot what this means, hopefully something like: Naughty user don't do that.
        return;
    }

    req.query.end = Math.min(parseInt(req.query.end), (Date.now() / 1000));

    let pointCount = 170000;
    const timeBetweenPoints = Math.floor((parseInt(req.query.end) - parseInt(req.query.start)) / pointCount);

    const data = Array.from({length: pointCount}, (x, y) => Math.abs(200 * Math.sin((y / pointCount) * 5 * 2 * Math.PI)));
    const time = Array.from({length: pointCount}, (x, index) => (index * timeBetweenPoints) + parseInt(req.query.start));

    console.log("Generated arrays, filling json request");
    res.send(JSON.stringify({
        times: time,
        values: data,
        decimation: 2
    }));
});

app.get('/alarms', (req, res) => {
    const StartDate = new Date(2019, 11, 23, 10, 53, 0, 0);

    const alarmData = [
        {secondsSinceEpoch: Math.round(new Date(2019, 11, 23, 10, 53, 0, 0).getTime() / 1000), value: 150},
        {secondsSinceEpoch: Math.round(new Date(2020, 1, 23, 10, 53, 0, 0).getTime() / 1000), value: 195},
        {secondsSinceEpoch: Math.round(StartDate.getTime() / 1000) + 3600000 * 10000, value: 195},
    ];

    if (req.query.channel !== undefined && req.query.type !== undefined) {
        res.send(JSON.stringify({
            alarms: alarmData
        }));
    }
});

app.get('/alerts', (req, res) => {
    const StartDate = new Date(2019, 11, 23, 10, 53, 0, 0);

    const alarmData = [
        {secondsSinceEpoch: Math.round(new Date(2019, 11, 23, 10, 53, 0, 0).getTime() / 1000), value: 130},
        {secondsSinceEpoch: Math.round(new Date(2020, 1, 23, 10, 53, 0, 0).getTime() / 1000), value: 170},
        {secondsSinceEpoch: Math.round(StartDate.getTime() / 1000) + 3600000 * 10000, value: 170}
    ];

    if (req.query.channel !== undefined && req.query.type !== undefined) {
        res.send(JSON.stringify({
            alarms: alarmData
        }));
    }
});

app.get('/states', (req, res) => {
    res.send(JSON.stringify({
        states: [
            {
                id: 1,
                name: "Unknown"
            },
            {
                id: 2,
                name: "LPR"
            },
            {
                id: 3,
                name: "Melt Down"
            }
        ]
    }));
});

app.post('/states', (req, res) => {
    if (req.query.name)
        console.log(`Registering the creation of a new State with the name: ${req.query.name}`);

    res.status(200);
});

app.post('/deleteState', (req, res) => {
    if (req.query.name)
        console.log(`Registering the deletion of a state with the name: ${req.query.name}`);

    res.status(200);
});

app.get('/', (req, res) => {
    res.send('Hello World!')
});

app.get('/earliestDataTime', (req, res) => {
    console.log("Got earliest time request");
    res.send(JSON.stringify({
        secondsSinceEpoch: 883656000
    }))
});

app.get('/latestDataTime', (req, res) => {
    console.log("Got latest time request");
    res.send(JSON.stringify({
        secondsSinceEpoch: Date.now() / 1000
    }));
});

app.post('/stateUpdate', (req, res) => {
    if (req.query.startTime && req.query.endTime && req.query.stateId && req.query.machine)
        console.log(`Registering that we entered state: ${(req.query.stateId)} at ${(req.query.startTime)} and exited at ${(req.query.endTime)} on machine ${req.query.machine}`);

    res.status(200);
});

app.listen(port, () => {
    console.log(`Example app listening at http://localhost:${port}`)
});