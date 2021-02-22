import 'date-fns';
import React, {useState} from 'react';
import {Grid, Typography} from "@material-ui/core";
import {NetworkAccess} from "../APIAccess/NetworkAccess";
import DateFnsUtils from '@date-io/date-fns';
import {KeyboardDatePicker, MuiPickersUtilsProvider} from '@material-ui/pickers';

const getMinTime = async () => {
    const network = new NetworkAccess();
    return (await network.getEarliestDataTime()) * 1000; // Interfaces all use seconds since epoch, javascript uses microseconds, so we convert upwards
}

const getMaxTime = async () => {
    const network = new NetworkAccess();
    return (await network.getLatestDataTime()) * 1000; // Interfaces all use seconds since epoch, javascript uses microseconds, so we convert upwards
}

export interface Props {
    startTime: number;
    endTime: number;
    updateStartTime: (newTime: number) => void;
    updateEndTime: (newTime: number) => void;
}

export const DateRangeSelector = (props: Props) => {
    const [minTime, setMinTime] = useState<number>(0);
    if (minTime === 0)
        getMinTime().then(time => {
            setMinTime(time);
            props.updateStartTime(time);
            console.log(`Loaded new min time of ${time}`);
        });

    const [maxTime, setMaxTime] = useState<number>(100);
    if (maxTime === 100)
        getMaxTime().then(time => {
            setMaxTime(time);
            props.updateEndTime(time);
        });

    return (
        <Grid container direction={"column"}>
            <Grid item>
                <Typography id="range-slider" gutterBottom>
                    Time range of data to display
                </Typography>
            </Grid>
            <Grid item>
                <MuiPickersUtilsProvider utils={DateFnsUtils}>
                    <Grid container direction={"row"} spacing={2}>
                        <Grid item>
                            <KeyboardDatePicker
                                margin="normal"
                                id="date-picker-dialog"
                                label="Data Display Start Time"
                                format="dd/MM/yyyy"
                                value={new Date(minTime)}
                                onChange={(date: Date | null) => props.updateStartTime(Math.round((date?.getTime() ?? 0) / 1000))}
                                minDate={new Date(minTime)}
                                maxDate={new Date(maxTime)}
                                KeyboardButtonProps={{
                                    'aria-label': 'change date',
                                }}
                            />
                        </Grid>
                        <Grid item>
                            <KeyboardDatePicker
                                margin="normal"
                                id="date-picker-dialog"
                                label="Data Display End Time"
                                format="dd/MM/yyyy"
                                value={new Date(maxTime)}
                                onChange={(date: Date | null) => props.updateEndTime(Math.round((date?.getTime() ?? 0) / 1000))}
                                minDate={new Date(minTime)}
                                maxDate={new Date(maxTime)}
                                KeyboardButtonProps={{
                                    'aria-label': 'change date',
                                }}
                            />
                        </Grid>
                    </Grid>
                </MuiPickersUtilsProvider>
            </Grid>
        </Grid>
    );
}