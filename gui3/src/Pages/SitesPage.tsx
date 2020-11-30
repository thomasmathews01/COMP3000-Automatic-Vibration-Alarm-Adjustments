import React, {useState} from 'react';
import GridOfAlarmStateItems from "../Components/GridOfAlarmStateItems";
import axios from "axios";
import {SingleAlarmStateItem} from "../Types/SingleAlarmStateItem";

interface serverSite {
    id: number,
    name: string,
}

const getSites = async (updateFn: (x: SingleAlarmStateItem[]) => void) => {
    try {
        const sites = await axios.get<{ sites: serverSite[] }>("http://localhost:3456/sites");

        updateFn(sites.data.sites.map((x) => {
            return {
                id: x.id,
                name: x.name,
                state: 1
            }
        }));
    } catch (exception) {
        console.log(exception.toString());
        return [];
    }
};

export const SitesPage = () => {
    const [sitesData, setSitesData] = useState<SingleAlarmStateItem[]>([]);

    if (sitesData.length === 0)
        getSites(setSitesData).then(() => console.log("Sites fetch completed. "));

    return (
        <GridOfAlarmStateItems items={sitesData} redirectString={"/site/"}/>
    );
}
