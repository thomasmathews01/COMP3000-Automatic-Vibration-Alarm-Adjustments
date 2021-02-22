import React from 'react';
import {Button, Card, CardContent, Grid, Typography} from "@material-ui/core";
import {CustomDataSet} from "../Types/CustomDataSet";
import {makeStyles} from "@material-ui/core/styles";
import Graph from "./Graph";

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

interface graphProps {
    typeID: number,
    channelID: number
}

function IndividualGraph(props: graphProps) {
    const classes = useStyles();
    return (
        <Card elevation={3}>
            <Typography className={classes.titleText}>{"Insert channel and type names here"}</Typography>
            <CardContent>
                <Graph type={props.typeID} channel={props.channelID}/>
            </CardContent>
        </Card>
    );
}


export interface Props {
    dataView: CustomDataSet;
    reConfigureFn: () => void;
}

export const CustomDataView = (props: Props) => {
    return (
        <Grid container spacing={2}>
            {
                props.dataView.dataSources.map((x) => (
                    <Grid item xs={12}>
                        <IndividualGraph typeID={x.type} channelID={x.channel}/>
                    </Grid>
                ))
            }
            <Grid item>
                <Card>
                    <Button variant={"contained"} onClick={props.reConfigureFn} color={"primary"}>
                        Reconfigure Data Sources
                    </Button>
                </Card>
            </Grid>
        </Grid>
    );
}
