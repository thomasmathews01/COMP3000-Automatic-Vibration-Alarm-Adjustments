import React from 'react';
import {Button, Grid, Typography} from "@material-ui/core";
import {DataSource} from "../Components/DataSourceSelector";
import {DataSources} from "../Components/DataSources";
import {DateRangeSelector} from "../Components/DateRangeSelector";
import {CustomDataSet} from "../Types/CustomDataSet"

export interface Props {
    finishedSettingData: () => void;
    dataDescription: CustomDataSet;
    updateDataDescription: (newDataDescription: CustomDataSet) => void;
}

export const DataSelection = (props: Props) => {
    const setTimeStart = (newTime: number) => {
        let newDescription = {...props.dataDescription};
        newDescription.startTimeEpochSeconds = newTime;
        props.updateDataDescription(newDescription);
    }

    const setTimeEnd = (newTime: number) => {
        let newDescription = {...props.dataDescription};
        newDescription.endTimeEpochSeconds = newTime;
        props.updateDataDescription(newDescription);
    }

    const setDataSources = (sources: DataSource[]) => {
        let newDescription = {...props.dataDescription};
        newDescription.dataSources = sources;
        props.updateDataDescription(newDescription);
    }

    return (
        <Grid container spacing={2} direction={"column"}>
            <Grid item xs={12}>
                <Typography id="range-slider">
                    Custom Data Selection
                </Typography>
            </Grid>
            <Grid item xs={12}>
                <Grid container spacing={2} direction={"row"}>
                    <Grid item xs={4}>
                        <DateRangeSelector startTime={props.dataDescription.startTimeEpochSeconds} endTime={props.dataDescription.endTimeEpochSeconds}
                                           updateStartTime={setTimeStart} updateEndTime={setTimeEnd}/>
                    </Grid>
                    <Grid item xs={8}>
                        <DataSources dataSources={props.dataDescription.dataSources} updateSources={newSources => setDataSources([...newSources])}/>
                    </Grid>
                </Grid>
            </Grid>
            <Grid item>
                <Button variant={"contained"} color={"primary"} onClick={props.finishedSettingData}>
                    View Data
                </Button>
            </Grid>
        </Grid>
    );
}
