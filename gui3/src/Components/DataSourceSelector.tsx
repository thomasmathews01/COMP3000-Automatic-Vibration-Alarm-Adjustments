import React from "react";
import {Grid, IconButton} from "@material-ui/core";
import {ChannelSelector} from "./ChannelSelector";
import {TypeSelector} from "./TypeSelector";
import {serverChannel, serverType} from "../APIAccess/ServerTypes";
import {RemoveCircle} from "@material-ui/icons";

export interface DataSource {
    channel: number;
    type: number;
}

export interface Props {
    updateSourceFn: (newSource: DataSource | null) => void,
    source: DataSource,
    allChannels: serverChannel[],
    allTypes: serverType[]
}

export const DataSourceSelector = (props: Props) => {
    const updateChannel = (newChannel: number) =>
        props.updateSourceFn({...props.source, ...{channel: newChannel}});

    const updateType = (newType: number) =>
        props.updateSourceFn({...props.source, ...{type: newType}});

    return (
        <Grid container direction={"row"} spacing={2}>
            <Grid item>
                <ChannelSelector channels={props.allChannels} selectedChannel={props.source.channel} updateFn={updateChannel}/>
            </Grid>
            <Grid item>
                <TypeSelector types={props.allTypes} selectedType={props.source.type} updateFn={updateType}/>
            </Grid>
            <Grid item>
                <IconButton color="primary" component="span" onClick={() => props.updateSourceFn(null)}>
                    <RemoveCircle/>
                </IconButton>
            </Grid>
        </Grid>
    );
}