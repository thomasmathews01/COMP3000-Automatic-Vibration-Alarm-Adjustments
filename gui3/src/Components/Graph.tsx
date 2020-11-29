import React, {useState} from 'react';
import {HorizontalGridLines, LineSeries, LineSeriesPoint, VerticalGridLines, XAxis, XYPlot, YAxis} from 'react-vis'
import '../../node_modules/react-vis/dist/style.css';
import {dataItem} from "../Types/DataTypes"; // TODO: Abstract network types into their own file to make API changes easier.
import axios from "axios";

const generateAreaData = () => {
    const sampleCount = 5000;
    return [
        {x: 0, y: 100},
        {x: sampleCount / 3, y: 100},
        {x: (sampleCount / 3) + 1, y: 0},
        {x: (2 * sampleCount / 3), y: 0},
        {x: (2 * sampleCount / 3) + 1, y: 100},
        {x: sampleCount - 1, y: 100},
    ];
}

const generateAreaData2 = () => {
    const sampleCount = 5000;
    return [
        {x: 0, y: 0},
        {x: sampleCount / 3, y: 0},
        {x: (sampleCount / 3) + 1, y: 100},
        {x: (2 * sampleCount / 3), y: 100},
        {x: (2 * sampleCount / 3) + 1, y: 0},
    ];
}

const fetchValueData = async (channel: number, type: number) => {
    const response = await axios.get<{ data: dataItem[] }>(`http://localhost:3456/data?channel=${channel}&type=${type}`);
    if (response.data.data)
        return response.data.data.map(item => {
            return {x: item.secondsSinceEpoch * 1000, y: item.value}
        });

    return [];
}

interface PropsT {
    channel: number,
    type: number
}

const Graph = (props: PropsT) => {
    const [valuesData, setValuesData] = useState<LineSeriesPoint[]>([]);

    if (valuesData.length === 0)
        fetchValueData(props.channel, props.type)
            .then((result) => setValuesData(result))
            .then(() => console.log("Fetch complete."));

    console.log("Rendering...");
    return (
        <XYPlot xType={"time"}
                width={850}
                height={300}>
            <HorizontalGridLines/>
            <VerticalGridLines/>
            <LineSeries data={valuesData}/>
            <XAxis tickLabelAngle={-90}/>
            <YAxis/>
        </XYPlot>
    );
}

export default Graph;

/*

            <AreaSeries data={generateAreaData()}/>
            <AreaSeries data={generateAreaData2()}/>
 */