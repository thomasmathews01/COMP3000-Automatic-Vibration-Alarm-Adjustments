import React, {useEffect, useState} from 'react';
import {AreaSeries, FlexibleWidthXYPlot, Highlight, HighlightArea, HorizontalGridLines, LineSeries, LineSeriesPoint, VerticalGridLines, XAxis, YAxis} from 'react-vis'
import '../../node_modules/react-vis/dist/style.css';
import {alarmLevelChange, dataItem} from "../Types/DataTypes"; // TODO: Abstract network types into their own file to make API changes easier.
import axios from "axios";
import {Grid} from "@material-ui/core";
import {GraphSettings} from "./GraphSettings";
import {State, statePeriod} from "../Types/StateTypes";

const fetchAlarmChanges = async (channel: number, type: number, alarmType: string) => {
    const response = await axios.get<{ alarms: alarmLevelChange[] }>(`http://localhost:3456/${alarmType}s?channel=${channel}&type=${type}`);

    if (response.data.alarms) {
        console.log(`Retrieved ${response.data.alarms} alarm levels of type: ${alarmType}`);
        return response.data.alarms.map(item => {
            return {x: item.secondsSinceEpoch * 1000, y: item.value}
        });
    }

    return [];
}

const fetchAlarmLevels = async (channel: number, type: number) => fetchAlarmChanges(channel, type, "alarm");
const fetchAlertLevels = async (channel: number, type: number) => fetchAlarmChanges(channel, type, "alert");

const fetchValueData = async (channel: number, type: number) => {
    const response = await axios.get<{ data: dataItem[] }>(`http://localhost:3456/data?channel=${channel}&type=${type}`);
    if (response.data.data)
        return response.data.data.map(item => {
            return {x: item.secondsSinceEpoch * 1000, y: item.value}
        });

    return [];
}

const fetchStates = async () => {
    const response = await axios.get<{ states: State[] }>(`http://localhost:3456/states`);
    return response.data.states ?? [];
}

const networkFetchStatePeriods = async (channel: number) => {
    const response = await axios.get<{ states: statePeriod[] }>(`http://localhost:3456/statePeriods?channel=${channel}`);
    return response.data.states ?? [];
}

const fetchStatePeriods = async (channel: number) => {
    const states = await networkFetchStatePeriods(channel);
    const maxStateId = Math.max(...states.map(x => x.id));
    const output = Array<LineSeriesPoint[]>(maxStateId - 1);

}

interface PropsT {
    channel: number,
    type: number
}

const Graph = (props: PropsT) => {
    const [valuesData, setValuesData] = useState<LineSeriesPoint[]>([]);
    const [currentArea, setCurrentArea] = useState<HighlightArea>({top: 100, bottom: -100, left: 0, right: new Date().getTime()}); // TODO: Set initials sensibly so we can cope with the initial lack of values
    const [inZoomMode, setZoomMode] = useState(true);
    const [states, setStates] = useState<State[]>([]);
    const [statePeriods, setStatePeriods] = useState<LineSeriesPoint[][]>([]);
    const [selectedState, setSelectedState] = useState<State | undefined>(undefined);
    const [alarmLevels, setAlarmLevels] = useState<LineSeriesPoint[]>([]);
    const [alertLevels, setAlertLevels] = useState<LineSeriesPoint[]>([]);

    useEffect(() => {
        fetchStates()
            .then(fetchedStates => setStates(fetchedStates)); // TODO: Also pull the actual state periods themselves
        fetchValueData(props.channel, props.type)
            .then((result) => {
                setValuesData(result);
                displayFullRange(result);
            });
        fetchAlertLevels(props.channel, props.type)
            .then((levels) => setAlertLevels(levels));
        fetchAlarmLevels(props.channel, props.type)
            .then((levels) => setAlarmLevels(levels));
        fetchStatePeriods(props.channel)
            .then(periods => setStatePeriods(periods));
    }, []);

    const handleStateSelection = (area: HighlightArea | null) => {
        if (area === null || area.left === undefined || area.right === undefined || selectedState === undefined)
            return;

        const leftBound = (area.left as any).getTime(); // The documentation suggests this should come through as a number. It doesn't, it comes through as a date.
        const rightBound = (area.right as any).getTime(); // The documentation suggests this should come through as a number. It doesn't, it comes through as a date.

        axios.post(`http://localhost:3456/stateUpdate?startTime=${leftBound}&endTime=${rightBound}&machine=${1}&stateId=${selectedState.id}`)
            .then(() => console.log("Pushed new state update")); // TODO: Non-static machine address and error handling
    }

    function updateDragArea(area: HighlightArea | null) {
        if (inZoomMode) {
            if (area !== undefined && area !== null && currentArea !== undefined && setCurrentArea !== undefined) {
                if (area.left !== undefined && area.right !== undefined && area.bottom !== undefined && area.top !== undefined) {
                    const fullRange = getFullRange(valuesData);
                    area.bottom = Math.max(area.bottom, fullRange.bottom);
                    area.top = Math.min(area.top, fullRange.top);
                    area.left = Math.max(area.left, fullRange.left);
                    area.right = Math.min(area.right, fullRange.right);
                }

                setCurrentArea(area ?? {});
            }
        } else handleStateSelection(area);
    }

    const getFullRange = (points: LineSeriesPoint[]) => {
        // TODO: This should take into the account the alarm and alert levels when calculating the maximum and minimum
        return {
            left: points[0].x, right: points[points.length - 1].x,
            bottom: Math.min.apply(null, points.map(p => p.y)),
            top: Math.max.apply(null, points.map(p => p.y))
        };
    }

    const displayFullRange = (points: LineSeriesPoint[]) => {
        setCurrentArea(getFullRange(points));
    }

    return (
        <Grid container>
            <Grid item xs={9} md={9} xl={9} lg={9}>
                <FlexibleWidthXYPlot xType={"time"}
                                     height={200}
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
                    {
                        statePeriods.map(period => (
                            <AreaSeries data={period}/>
                        ))
                    }
                    <HorizontalGridLines/>
                    <VerticalGridLines/>
                    <LineSeries data={valuesData} color="black"/>
                    <LineSeries data={alarmLevels} color="red" strokeStyle="dashed"/>
                    <LineSeries data={alertLevels} color="yellow" strokeStyle="dashed"/>
                    <XAxis tickLabelAngle={-90}/>
                    <YAxis/>
                    <Highlight drag={true} enableY={inZoomMode} onDragEnd={updateDragArea}/>
                </FlexibleWidthXYPlot>
            </Grid>
            <Grid item xs={3} md={3} xl={3} lg={3}>
                <GraphSettings currentZoomMode={inZoomMode}
                               updateZoomMode={setZoomMode}
                               resetZoom={() => displayFullRange(valuesData)}
                               states={states}
                               currentSelectedState={selectedState}
                               setSelectedState={setSelectedState}
                />
            </Grid>
        </Grid>
    );
}

export default Graph;
