import React, {useState} from 'react';
import {Card, CardContent, Grid, Typography} from "@material-ui/core";
import {makeStyles} from "@material-ui/core/styles";
import axios from "axios";
import OuterPage from "../Components/OuterPage";

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

interface type {
    id: number,
    name: string
}


function IndividualGraph(props: graphProps) {
    const classes = useStyles();
    return (
        <Card elevation={3}>
            <Typography className={classes.titleText}>{props.type.name}</Typography>
            <CardContent>
            </CardContent>
        </Card>
    );
}

const fetchGraphTypes = async (): Promise<type[]> => {
    try {
        const resp = await axios.get<{ types: type[] }>(`http://localhost:3456/dataTypes?channel=${1}`);

        return resp.data.types;
    } catch (exception) {
        console.log(exception.toString());
        return [];
    }
}

export default function IndividualChannel() {
    const classes = useStyles();

    const [GraphTypes, setGraphTypes] = useState<type[]>([]);

    if (GraphTypes.length === 0)
        fetchGraphTypes().then(x => setGraphTypes(x));

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
                {
                    GraphTypes.map(x => (
                        <Grid item xs={6}>
                            <IndividualGraph type={x} channelID={1}/>
                        </Grid>
                    ))
                }
            </Grid>
        </div>
    );
}