import React, {useState} from 'react';
import {SingleAlarmStateItem} from "../Types/SingleAlarmStateItem";
import axios from "axios";
import OuterPage from "../Components/OuterPage";
import GridOfAlarmStateItems from "../Components/GridOfAlarmStateItems";
import {useParams} from "react-router";

interface serverChannel {
    id: number,
    name: string,
    units: string
}

const getChannels = async (machineID: number) => {
    try {
        const resp = await axios.get<{ channels: serverChannel[] }>(`http://localhost:3456/channels?machine=${machineID}`);

        return resp.data.channels.map((x) => {
            return {
                id: x.id,
                name: x.name,
                state: 1
            }
        });
    } catch (exception) {
        console.log(exception.toString());
        return [];
    }
};

const startChannelRefreshInterval = (siteID: number, updateFn: (x: SingleAlarmStateItem[]) => void) => {
    return setInterval(async () => {
        const newMachines = await getChannels(siteID);
        updateFn(newMachines);
    }, 1000); // Replace this with nicer server side rendering to get the props each time from the real server
}

export default function IndividualMachine() {
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
        <div>
            <OuterPage/>
            <GridOfAlarmStateItems items={channelsData} redirectString={"/channel/"}/>
        </div>
    );
}
