import React, {useEffect, useState} from 'react';
import {AreaSeries, FlexibleWidthXYPlot, Highlight, HighlightArea, HorizontalGridLines, LineSeries, LineSeriesPoint, VerticalGridLines, XAxis, YAxis} from 'react-vis'
import '../../node_modules/react-vis/dist/style.css';
import {Grid} from "@material-ui/core";
import {GraphSettings} from "./GraphSettings";
import {State} from "../Types/StateTypes";
import {NetworkAccess} from "../APIAccess/NetworkAccess";

const networkAccess = new NetworkAccess("http://localhost:1234");

const fetchAlarmLevels = async (channel: number, type: number) => networkAccess.fetchAlarmChanges(channel, type, "alarm");
const fetchAlertLevels = async (channel: number, type: number) => networkAccess.fetchAlarmChanges(channel, type, "alert");

const fetchStatePeriods = async (channel: number) => {
    const states = await networkAccess.networkFetchStatePeriods(channel);
    const maxStateId = Math.max(...states.map(x => x.id));

    return Array<LineSeriesPoint[]>(maxStateId - 1);
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
        networkAccess.fetchStates()
            .then(fetchedStates => setStates(fetchedStates)); // TODO: Also pull the actual state periods themselves
        networkAccess.fetchValueData(props.channel, props.type)
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

        const networkAccess = new NetworkAccess("http://localhost:1234");
        networkAccess.issueStateUpdate(leftBound, rightBound, selectedState.id)
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
        // Not that, theoretically, the alarm or alert levels should ever be substantially higher that the real level
        // given that they are being calculated as n sigma against the average level.
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
