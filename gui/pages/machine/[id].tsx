import React, {useState} from 'react';
import OuterPage from "../OuterPage";
import GridOfAlarmStateItems from "../GridOfAlarmStateItems";
import axios from "axios";
import {SingleAlarmStateItem} from "../../Types/AlarmStateItem";
import {useRouter} from "next/router";

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

export default function IndividualSite() {
    const [channelsData, setChannelsData] = useState<SingleAlarmStateItem[]>([]);
    const [timer, setTimer] = useState<NodeJS.Timeout>();
    const router = useRouter();
    const {id} = router.query;

    if (timer === undefined && id as string !== undefined)
        setTimer(startChannelRefreshInterval(parseInt(id as string), setChannelsData));

    return (
        <div>
            <OuterPage/>
            <GridOfAlarmStateItems items={channelsData} redirectString={"/machine/"}/>
        </div>
    );
}
