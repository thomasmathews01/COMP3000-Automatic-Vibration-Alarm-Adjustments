import React, {useState} from 'react';
import {AlarmState, SingleAlarmStateItem} from "../Types/SingleAlarmStateItem";
import GridOfAlarmStateItems from "../Components/GridOfAlarmStateItems";
import {useParams} from "react-router";
import {NetworkAccess} from "../APIAccess/NetworkAccess";
import {serverChannel} from "../APIAccess/ServerTypes";

const channelToAlarmStateItem = (channel: serverChannel) => {
    return {
        id: channel.id,
        name: channel.name,
        state: AlarmState.NotInAlarm
    }
}

const startChannelRefreshInterval = (siteID: number, updateFn: (x: SingleAlarmStateItem[]) => void) => {
    const networkAccess = new NetworkAccess();

    return setInterval(async () => {
        const newChannels = await networkAccess.getChannels(siteID);
        updateFn(newChannels.map(channelToAlarmStateItem));
    }, 1000); // Replace this with nicer server side rendering to get the props each time from the real server
}

export const MachinePage = () => {
    const [channelsData, setChannelsData] = useState<SingleAlarmStateItem[]>([]);
    const [timer, setTimer] = useState<NodeJS.Timeout>();
    const {id} = useParams<Record<string, string | undefined>>()

    if (id === undefined) {
        console.log("Type ID was undefined");
        return <></>;
    }

    if (timer === undefined && id as string !== undefined)
        setTimer(startChannelRefreshInterval(parseInt(id as string), setChannelsData));

    return (
            <GridOfAlarmStateItems items={channelsData} redirectString={"/channel/"}/>
    );
}
