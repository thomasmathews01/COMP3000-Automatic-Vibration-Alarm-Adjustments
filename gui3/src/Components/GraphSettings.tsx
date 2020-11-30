import React from 'react';
import {Button, FormControlLabel, Grid, Radio, RadioGroup, Typography} from "@material-ui/core";
import {makeStyles} from "@material-ui/core/styles";
import {State} from "../Types/StateTypes";

const useStyles = makeStyles((theme) => ({
    resetButton: {
        background: theme.palette.secondary.main
    },
    secondaryHeader: {
        display: 'flex',
        alignText: 'center'
    },
    majorGridSection: {
        marginBottom: 10
    }
}));

interface propsT {
    updateZoomMode: (x: boolean) => void,
    currentZoomMode: boolean,
    resetZoom: () => void
    states: State[],
    currentSelectedState: State | undefined,
    setSelectedState: (x: State | undefined) => void
}

export const GraphSettings = (props: propsT) => {
    const classes = useStyles();

    return (
        <Grid container alignItems="center" justify="center" direction="column">
            <Grid item id="InteractionModeSelection" className={classes.majorGridSection}>
                <Grid container direction="column" alignItems="center" justify="center">
                    <Grid item xs={12} md={12} xl={12} lg={12}>
                        <Typography>{"Operating Mode"}</Typography>
                    </Grid>
                    <Grid item>
                        <RadioGroup row aria-label="state" name="state" value={props.currentZoomMode} onChange={(e) => props.updateZoomMode(e.target.value === "true")}>
                            <FormControlLabel value={true} control={<Radio/>} label={"Zoom Mode"}/>
                            <FormControlLabel value={false} control={<Radio/>} label={"State Select Mode"}/>
                        </RadioGroup>
                    </Grid>
                </Grid>
            </Grid>
            <Grid item id="StateSelectors" hidden={props.currentZoomMode} className={classes.majorGridSection}>
                <Grid container direction="column" alignItems="center" justify="center">
                    <Grid item xs={12} md={12} xl={12} lg={12}>
                        <Typography>{"Selected State"}</Typography>
                    </Grid>
                    <RadioGroup row aria-label="state" name="state" value={props.currentSelectedState?.name}
                                onChange={(e) => props.setSelectedState(props.states.find(y => y.name === e.target.value))}>
                        {
                            props.states.map(x => (
                                <FormControlLabel value={x.name} control={<Radio/>} label={x.name}/>
                            ))
                        }
                    </RadioGroup>
                </Grid>
            </Grid>
            <Grid item id="ZoomReset" className={classes.majorGridSection}>
                <Button className={classes.resetButton} onClick={() => props.resetZoom()}>Reset Zoom</Button>
            </Grid>
        </Grid>
    );
};
