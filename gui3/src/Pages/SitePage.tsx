import React, {useState} from 'react';
import {SingleAlarmStateItem} from "../Types/SingleAlarmStateItem";
import GridOfAlarmStateItems from "../Components/GridOfAlarmStateItems";
import {useParams} from "react-router";
import {NetworkAccess} from "../APIAccess/NetworkAccess";

const startMachineRefreshInterval = (siteID: number, updateFn: (x: SingleAlarmStateItem[]) => void) => {
    const networkAccess = new NetworkAccess("http://localhost:3456")

    return setInterval(async () => {
        const newMachines = await networkAccess.getMachines(siteID);
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
