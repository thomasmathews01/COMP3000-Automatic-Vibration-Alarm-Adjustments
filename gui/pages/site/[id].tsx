import React, {useState} from 'react';
import OuterPage from "../OuterPage";
import GridOfAlarmStateItems from "../GridOfAlarmStateItems";
import axios from "axios";
import {SingleAlarmStateItem} from "../../Types/AlarmStateItem";
import {useRouter} from "next/router";

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

export default function IndividualSite() {
    const [machinesData, setMachinesData] = useState<SingleAlarmStateItem[]>([]);
    const [timer, setTimer] = useState<NodeJS.Timeout>();
    const router = useRouter();
    const {id} = router.query;

    if (timer === undefined && id as string !== undefined)
        setTimer(startMachineRefreshInterval(parseInt(id as string), setMachinesData));

    return (
        <div>
            <OuterPage/>
            <GridOfAlarmStateItems items={machinesData} redirectString={"/machine/"}/>
        </div>
    );
}
