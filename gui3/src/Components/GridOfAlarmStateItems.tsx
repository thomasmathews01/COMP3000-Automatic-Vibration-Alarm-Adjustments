import React from 'react';
import {Grid} from "@material-ui/core";
import {SingleAlarmStateItem} from "../Types/SingleAlarmStateItem";
import AlarmStateItem from "./AlarmStateItem";

const GridOfAlarmStateItems = (props: { items: SingleAlarmStateItem[], redirectString: string }) => {
    return (
        <Grid container direction="row" spacing={3}>
            {
                props.items.map((x: SingleAlarmStateItem) => (
                    <Grid item xs={3} md={3}>
                        <AlarmStateItem item={x} redirectString={props.redirectString}/>
                    </Grid>
                ))
            }
        </Grid>
    );
};

export default GridOfAlarmStateItems;