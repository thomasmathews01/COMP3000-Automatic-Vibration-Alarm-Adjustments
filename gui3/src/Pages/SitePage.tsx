import React, {useState} from 'react';
import {SingleAlarmStateItem} from "../Types/SingleAlarmStateItem";
import GridOfAlarmStateItems from "../Components/GridOfAlarmStateItems";
import {useParams} from "react-router";
import {NetworkAccess} from "../APIAccess/NetworkAccess";
import {useAsync} from "react-async-hook";

const getMachinesForSite = async (siteID: number) => {
    const networkAccess = new NetworkAccess();
    return await networkAccess.getMachines(siteID);
}

export const SitePage = () => {
    const [machinesData, setMachinesData] = useState<SingleAlarmStateItem[]>([]);
    const {id} = useParams<Record<string, string | undefined>>()
    const asyncMachines = useAsync(getMachinesForSite, [parseInt(id ?? "1")]);

    if (machinesData.length === 0 && !asyncMachines.loading && !asyncMachines.error && asyncMachines.result)
        setMachinesData(asyncMachines.result);

    return (
        <GridOfAlarmStateItems items={machinesData} redirectString={"/machine/"}/>
    );
}
