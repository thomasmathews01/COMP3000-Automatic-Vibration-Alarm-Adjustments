import React, {useState} from 'react';
import OuterPage from "./OuterPage";
import GridOfAlarmStateItems from "./GridOfAlarmStateItems";
import axios from "axios";
import {SingleAlarmStateItem} from "../Types/AlarmStateItem";

interface serverSite {
    id: number,
    name: string,
}

const getSites = async () => {
    try {
        const sites = await axios.get<{ sites: serverSite[] }>("http://localhost:3456/sites");

        return sites.data.sites.map((x) => {
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

const startSiteRefreshInterval = (updateFn: (x: SingleAlarmStateItem[]) => void) => {
    return setInterval(async () => {
        const newSites = await getSites();
        updateFn(newSites);
    }, 1000); // Replace this with nicer server side rendering to get the props each time from the real server
}

export default function Sites() {
    const [sitesData, setSitesData] = useState<SingleAlarmStateItem[]>([]);
    const [timer, setTimer] = useState<NodeJS.Timeout>();

    if (timer === undefined)
        setTimer(startSiteRefreshInterval(setSitesData));

    return (
        <div>
            <OuterPage/>
            <GridOfAlarmStateItems items={sitesData} redirectString={"/site/"}/>
        </div>
    );
}

//TODO: Replace icon.