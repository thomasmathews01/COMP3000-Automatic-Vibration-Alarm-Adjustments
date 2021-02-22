import {MenuItem, Select} from "@material-ui/core";
import React from "react";
import {serverChannel} from "../APIAccess/ServerTypes";

export interface Props {
    updateFn: (newChannelID: number) => void,
    selectedChannel: number,
    channels: serverChannel[]
}

const getMenuItemFromChannel = (channel: serverChannel) => {
    return (
        <MenuItem value={channel.id}>{channel.name}</MenuItem>
    );
}

export const ChannelSelector = (props: Props) => {
    return (
        <Select
            value={props.selectedChannel}
            onChange={(e) => props.updateFn(e.target.value as number)}
            displayEmpty
            aria-label={"LABEL"}
            fullWidth={true}
        >
            {
                props.channels.map(getMenuItemFromChannel)
            }
        </Select>
    );
}