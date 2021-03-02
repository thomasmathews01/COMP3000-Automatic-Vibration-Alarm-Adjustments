const express = require('express');
const cors = require('cors');
const {json, urlencoded} = require("body-parser");

const port = 1234;

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

    res.send(JSON.stringify({types: typeInformation}));
});

app.get('/data', async (req, res) => {
    if (!req.query.start || !req.query.end || !req.query.channel || !req.query.type) {
        console.log("Data Requested badly!");
        res.status(400); // Forgot what this means, hopefully something like: Naughty user don't do that.
        return;
    }

    req.query.end = Math.min(parseInt(req.query.end), (Date.now() / 1000));

    let pointCount = 170000;
    const timeBetweenPoints = Math.floor((parseInt(req.query.end) - parseInt(req.query.start)) / pointCount);

    res.send(JSON.stringify({
        times: Array.from({length: pointCount}, (x, index) => (index * timeBetweenPoints) + parseInt(req.query.start)),
        values: Array.from({length: pointCount}, (x, y) => Math.abs(200 * Math.sin((y / pointCount) * 5 * 2 * Math.PI)))
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

app.get('/typeInformation', (req, res) => {
    console.log("Got type information request");
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

    const type = typeInformation.find(x => x.id === parseInt(req.query.type_id));

    res.send(JSON.stringify({
        id: type.id,
        name: type.name
    }));
});

app.get('/channelInformation', (req, res) => {
    console.log("Got channel information request");

    res.send(JSON.stringify({
        id: parseInt(req.query.channel_id),
        name: `Channel ${parseInt(req.query.channel_id)}`
    }));
});

app.get('/statePeriods', (req, res) => {
    console.log(`Got state periods request for channel ${req.query.channels}`);

    res.send(JSON.stringify({
        states: [
            {
                id: 1,
                start_seconds_since_epoch: 883656000,
                end_seconds_since_epoch: 983656000
            },
            {
                id: 2,
                start_seconds_since_epoch: 983656000,
                end_seconds_since_epoch: 1283656000
            },
            {
                id: 1,
                start_seconds_since_epoch: 1283656000,
                end_seconds_since_epoch: Date.now() / 1000
            }
        ]
    }));
});

app.post('/stateUpdate', (req, res) => {
    if (req.query.startTime && req.query.endTime && req.query.stateId && req.query.machine)
        console.log(`Registering that we entered state: ${(req.query.stateId)} at ${(req.query.startTime)} and exited at ${(req.query.endTime)} on machine ${req.query.machine}`);

    res.status(200);
});

function getRandomInt(max) {
    return Math.floor(Math.random() * Math.floor(max));
}

app.get('/currentAlarmState', (req, res) => {
    const possibleStates = ['alarm', 'alert', 'none'];
    const response = JSON.stringify({
        state: possibleStates[getRandomInt(3)]
    });

    if (req.query.site_id) {
        console.log(`Sending a site ${req.query.site_id} alarm state of ${response} `);
    }
    if (req.query.machine_id) {
        console.log(`Sending a machine ${req.query.machine_id} alarm state of ${response} `);
    }
    if (req.query.channel_id) {
        console.log(`Sending a channel ${req.query.channel_id} alarm state of ${response} `);
    }

    res.send(response);
})

app.listen(port, () => {
    console.log(`Example app listening at http://localhost:${port}`)
});