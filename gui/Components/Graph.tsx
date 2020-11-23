import React, {useState} from "react";
import {Area, ComposedChart, Label, Legend, Line, Tooltip, XAxis, YAxis} from 'recharts';
import axios from "axios";
import {alarmLevelChange, dataItem} from "../Types/DataTypes";
import {addAlarmData, addStateData} from "./CombineDataSources";

/*const data = [
    {time: 0, Level: 100, Alert: 100, Alarm: 170, Unknown: 200, "Low Power Refuelling": undefined},
    {time: 2, Level: 110, Alert: 100, Alarm: 170, Unknown: 200, "Low Power Refuelling": undefined},
    {time: 3, Level: 140, Alert: 100, Alarm: 170, Unknown: 200, "Low Power Refuelling": undefined},
    {time: 4, Level: 10, Alert: 100, Alarm: 170, Unknown: 200, "Low Power Refuelling": undefined},
    {time: 5, Level: 100, Alert: 100, Alarm: 170, Unknown: 200, "Low Power Refuelling": undefined},
    // {time: 6, Level: undefined, Alert: 100, Alarm: 170}
    {time: 20, Level: 120, Alert: 100, Alarm: 195, Unknown: 200, "Low Power Refuelling": 200},
    {time: 21, Level: 150, Alert: 100, Alarm: 195, Unknown: undefined, "Low Power Refuelling": 200},
    {time: 22, Level: 120, Alert: 100, Alarm: 195, Unknown: undefined, "Low Power Refuelling": 200},
    {time: 23, Level: 130, Alert: 100, Alarm: 195, Unknown: undefined, "Low Power Refuelling": 200},
    {time: 24, Level: 140, Alert: 100, Alarm: 195, Unknown: undefined, "Low Power Refuelling": 200},
    {time: 25, Level: 200, Alert: 100, Alarm: 195, Unknown: undefined, "Low Power Refuelling": 200},
    {time: 26, Level: 120, Alert: 100, Alarm: 195, Unknown: 200, "Low Power Refuelling": 200},
    {time: 27, Level: 125, Alert: 100, Alarm: 195, Unknown: 200, "Low Power Refuelling": undefined},
    {time: 28, Level: 118, Alert: 100, Alarm: 195, Unknown: 200, "Low Power Refuelling": undefined}
];*/

interface propsT {
    channelID: number,
    typeID: number
}

const fetchData = async (channel: number, type: number) => {
    try {
        let responses = await Promise.all(
            [axios.get<{ data: dataItem[] }>(`http://localhost:3456/data?channel=${channel}&type=${type}`),
                axios.get<{ data: alarmLevelChange[] }>(`http://localhost:3456/alarms?channel=${channel}&type=${type}`),
                axios.get<{ data: alarmLevelChange[] }>(`http://localhost:3456/alerts?channel=${channel}&type=${type}`)]
        );

        console.log(`Received Data: ${JSON.stringify(responses[0].data.data)}`);
        console.log(`Received Alarms: ${JSON.stringify(responses[1].data.data)}`);
        console.log(`Received Alerts: ${JSON.stringify(responses[2].data.data)}`);

        return addStateData(await addAlarmData(responses[0].data.data, responses[1].data.data, responses[2].data.data));
    } catch (exception) {
        console.log(exception.toString());
        return [];
    }
}

export default function Graph(props: propsT) {

    const [data, setData] = useState<dataItem[]>([]);
    if (data.length == 0)
        fetchData(props.channelID, props.typeID).then(x => setData(x));


    return (
        <ComposedChart width={550} height={300} data={data}>
            <Area legendType={"square"} type="monotone" dataKey="States[0].value" fill="#d3d3d3" stroke="#d3d3d3" dot={false}/> {/* Light grey */}

            <Line strokeWidth={2} type="monotone" dataKey="value" stroke="#8884d8" dot={false}/>
            <Tooltip/>
            <Line strokeDasharray="10 4" strokeWidth={2} type="monotone" dataKey="AlertLevel" stroke="#FFA500" dot={false}/>
            <Line strokeDasharray="10 4" strokeWidth={2} type="monotone" dataKey="AlarmLevel" stroke="#ff0808" dot={false}/>
            <XAxis domain={['auto', 'auto']} type='number' dataKey="time" tickFormatter={timeStr => new Date(timeStr * 1000).toDateString()}>
                <Label value={"Elapsed Time (Seconds)"} position={"insideBottom"} offset={-30}/>
            </XAxis>
            <YAxis domain={['auto', 'auto']} type='number' dataKey="value" unit={"MW"}>
                <Label value={"Levels (MW)"} position={"left"} offset={5} angle={-90}/>
            </YAxis>
            <Legend margin={{top: 100}}/>
        </ComposedChart>
    );
}

// <Area legendType={"square"} type="monotone" dataKey="Low Power Refuelling" fill="#add8e6" stroke="#add8e6" dot={false}/>  Light blue
