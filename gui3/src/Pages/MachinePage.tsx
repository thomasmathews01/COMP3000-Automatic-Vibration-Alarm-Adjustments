import React, {useState} from 'react';
import {AlarmState, SingleAlarmStateItem} from "../Types/SingleAlarmStateItem";
import GridOfAlarmStateItems from "../Components/GridOfAlarmStateItems";
import {useParams} from "react-router";
import {NetworkAccess} from "../APIAccess/NetworkAccess";
import {serverChannel} from "../APIAccess/ServerTypes";
import {useAsync} from "react-async-hook";

const channelToAlarmStateItem = (channel: serverChannel) => {
    return {
        id: channel.id,
        name: channel.name,
        state: AlarmState.NotInAlarm
    }
}

const getChannels = async (machineID: number) => {
    const networkAccess = new NetworkAccess();
    return (await networkAccess.getChannels(machineID)).map(channelToAlarmStateItem);
}

export const MachinePage = () => {
    const [channelsData, setChannelsData] = useState<SingleAlarmStateItem[]>([]);
    const {id} = useParams<Record<string, string | undefined>>()

    const asyncChannels = useAsync(getChannels, [parseInt(id ?? "1")]);
    if (channelsData.length === 0 && !asyncChannels.loading && !asyncChannels.error && asyncChannels.result)
        setChannelsData(asyncChannels.result);

    return (
        <GridOfAlarmStateItems items={channelsData} redirectString={"/channel/"}/>
    );
}
