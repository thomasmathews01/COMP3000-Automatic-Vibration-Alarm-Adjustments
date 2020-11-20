import React from 'react';
import Graph from "../../Components/Graph";
import {Card, CardContent, Grid, Typography} from "@material-ui/core";
import OuterPage from "../../Components/OuterPage";
import {makeStyles} from "@material-ui/core/styles";

const useStyles = makeStyles(() => ({
    titleText: {
        textAlign: "center",
        fontSize: 30,
    },
    pageBackground: {
        color: "grey",
        background: "grey",
        height: "100vh"
    }
}));

interface propsT {
    title: string;
}

function IndividualGraph(props: propsT) {
    const classes = useStyles();
    return (
        <Card elevation={3}>
            <Typography className={classes.titleText}>{props.title}</Typography>
            <CardContent>
                <Graph/>
            </CardContent>
        </Card>
    );
}

export default function IndividualChannel() {
    const classes = useStyles();
    return (
        <div className={classes.pageBackground}>
            <OuterPage/>
            <Grid container spacing={2}>
                <Grid item xs={12}>
                    <Card>
                        <CardContent>
                            <Typography className={classes.titleText}>{"TG 7 - Bearing 1 Vibration"}</Typography>
                        </CardContent>
                    </Card>
                </Grid>
                <Grid item xs={6}>
                    <IndividualGraph title={"RMS"}/>
                </Grid>
                <Grid item xs={6}>
                    <IndividualGraph title={"Pk2Pk"}/>
                </Grid>
                <Grid item xs={6}>
                    <IndividualGraph title={"DC"}/>
                </Grid>
            </Grid>
        </div>
    );
}