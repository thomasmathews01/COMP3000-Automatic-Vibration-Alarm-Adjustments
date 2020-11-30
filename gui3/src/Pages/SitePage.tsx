import React, {useState} from 'react';
import axios from "axios";
import {SingleAlarmStateItem} from "../Types/SingleAlarmStateItem";
import GridOfAlarmStateItems from "../Components/GridOfAlarmStateItems";
import {useParams} from "react-router";

interface serverMachine {
    id: number,
    name: string,
}

const getMachines = async (siteID: number) => {
    try {
        const resp = await axios.get<{ machines: serverMachine[] }>(`http://localhost:3456/machines?site=${siteID}`);

        return resp.data.machines.map((x) => {
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

const startMachineRefreshInterval = (siteID: number, updateFn: (x: SingleAlarmStateItem[]) => void) => {
    return setInterval(async () => {
        const newMachines = await getMachines(siteID);
        updateFn(newMachines);
    }, 1000); // Replace this with nicer server side rendering to get the props each time from the real server
}

export const SitePage = () => {
    const [machinesData, setMachinesData] = useState<SingleAlarmStateItem[]>([]);
    const [timer, setTimer] = useState<NodeJS.Timeout>();
    const {id} = useParams<Record<string, string | undefined>>()

    if (id === undefined) {
        console.log("Type ID was undefined");
        return <></>;
    }

    if (timer === undefined && id as string !== undefined)
        setTimer(startMachineRefreshInterval(parseInt(id as string), setMachinesData));

    return (
            <GridOfAlarmStateItems items={machinesData} redirectString={"/machine/"}/>
    );
}
