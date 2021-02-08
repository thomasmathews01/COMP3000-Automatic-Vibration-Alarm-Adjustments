import React, {useState} from 'react';
import {Card, CardContent, Grid, Typography} from "@material-ui/core";
import {makeStyles} from "@material-ui/core/styles";
import Graph from "../Components/Graph";
import {NetworkAccess} from '../APIAccess/NetworkAccess'
import {type} from '../APIAccess/ServerTypes'

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
    type: type,
    channelID: number
}

function IndividualGraph(props: graphProps) {
    const classes = useStyles();
    return (
        <Card elevation={3}>
            <Typography className={classes.titleText}>{props.type.name}</Typography>
            <CardContent>
                <Graph type={props.type.id} channel={props.channelID}/>
            </CardContent>
        </Card>
    );
}

export const ChannelPage = () => {
    const classes = useStyles();
    const networkAccess = new NetworkAccess("http://localhost:1234");

    const [GraphTypes, setGraphTypes] = useState<type[]>([]);

    if (GraphTypes.length === 0)
        networkAccess.fetchGraphTypes().then(x => setGraphTypes(x));

    console.log(`Known graph types: ${GraphTypes.length}`);

    return (
        <Grid container spacing={2}>
            <Grid item xs={12}>
                <Card>
                    <CardContent>
                        <Typography className={classes.titleText}>{"TG 7 - Bearing 1 Vibration"}</Typography>
                    </CardContent>
                </Card>
            </Grid>
            {
                GraphTypes.map(x => (
                    <Grid item xs={12}>
                        <IndividualGraph type={x} channelID={1}/>
                    </Grid>
                    ))
                }
            </Grid>
    );
}