import React from "react";
import {Area, ComposedChart, Label, Legend, Line, XAxis, YAxis} from 'recharts';

const data = [
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
];

export default function Graph() {
    return (
        <ComposedChart width={550} height={300} data={data}>
            <Area legendType={"square"} type="monotone" dataKey="Unknown" fill="#d3d3d3" stroke="#d3d3d3" dot={false}/> {/* Light grey */}
            <Area legendType={"square"} type="monotone" dataKey="Low Power Refuelling" fill="#add8e6" stroke="#add8e6" dot={false}/> {/* Light blue */}
            <Line strokeWidth={2} type="monotone" dataKey="Level" stroke="#8884d8"/>
            <Line strokeDasharray="10 4" strokeWidth={2} type="monotone" dataKey="Alert" stroke="#FFA500" dot={false}/>
            <Line strokeDasharray="10 4" strokeWidth={2} type="monotone" dataKey="Alarm" stroke="#ff0808" dot={false}/>
            <XAxis domain={['auto', 'auto']} type='number' dataKey="time" unit={'s'}>
                <Label value={"Elapsed Time (Seconds)"} position={"insideBottom"} offset={-30}/>
            </XAxis>
            <YAxis domain={['auto', 'auto']} type='number' dataKey="Level" unit={"MW"}>
                <Label value={"Levels (MW)"} position={"left"} offset={5} angle={-90}/>
            </YAxis>
            <Legend margin={{top: 100}}/>
        </ComposedChart>
    );
}
