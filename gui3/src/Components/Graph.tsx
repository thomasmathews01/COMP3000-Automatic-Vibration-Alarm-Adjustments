import React, {useState} from 'react';
import {Highlight, HighlightArea, HorizontalGridLines, LineSeries, LineSeriesPoint, VerticalGridLines, XAxis, XYPlot, YAxis} from 'react-vis'
import '../../node_modules/react-vis/dist/style.css';
import {dataItem} from "../Types/DataTypes"; // TODO: Abstract network types into their own file to make API changes easier.
import axios from "axios";
import {Button} from "@material-ui/core";
import {makeStyles} from "@material-ui/core/styles";

const useStyles = makeStyles((theme) => ({
    button: {
        background: theme.palette.secondary.main,
    }
}));

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
    const [currentArea, setCurrentArea] = useState<HighlightArea>({top: 100, bottom: -100, left: 0, right: new Date().getTime()}); // TODO: Set initials sensibly so we can cope with the initial lack of values
    const classes = useStyles();


    function updateDragArea(area: HighlightArea | null) {
        if (area !== undefined && area !== null && currentArea !== undefined && setCurrentArea !== undefined) {
            if (area.left !== undefined && area.right !== undefined && area.bottom !== undefined && area.top !== undefined) {
                const fullRange = getFullRange(valuesData);
                area.bottom = Math.max(area.bottom, fullRange.bottom);
                area.top = Math.max(area.top, fullRange.top);
                area.left = Math.max(area.left, fullRange.left);
                area.right = Math.max(area.right, fullRange.right);
            }

            setCurrentArea(area ?? {});
        }
    }

    const getFullRange = (points: LineSeriesPoint[]) => {
        return {
            left: points[0].x, right: points[points.length - 1].x,
            bottom: Math.min.apply(null, points.map(p => p.y)),
            top: Math.max.apply(null, points.map(p => p.y))
        };
    }

    const displayFullRange = (points: LineSeriesPoint[]) => {
        setCurrentArea(getFullRange(points));
    }

    if (valuesData.length === 0)
        fetchValueData(props.channel, props.type)
            .then((result) => {
                setValuesData(result);
                displayFullRange(result);
            })
            .then(() => console.log("Fetch complete."))

    console.log("Rendering...");
    return (
        <div>
            <XYPlot xType={"time"}
                    width={850}
                    height={300}
                    xDomain={
                        currentArea && [
                            currentArea.left,
                            currentArea.right
                        ]
                    }
                    yDomain={
                        currentArea && [
                            currentArea.bottom,
                            currentArea.top
                        ]
                    }>
                <HorizontalGridLines/>
                <VerticalGridLines/>
                <LineSeries data={valuesData}/>
                <XAxis tickLabelAngle={-90}/>
                <YAxis/>
                <Highlight drag={true} onDragEnd={updateDragArea}/>
            </XYPlot>
            <br/>
            <Button onClick={() => displayFullRange(valuesData)} className={classes.button}>Reset Zoom</Button>
        </div>
    );
}

export default Graph;

/*
            <AreaSeries data={generateAreaData()}/>
            <AreaSeries data={generateAreaData2()}/>
 */