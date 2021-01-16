import React, {useState} from 'react';
import GridOfAlarmStateItems from "../Components/GridOfAlarmStateItems";
import {SingleAlarmStateItem} from "../Types/SingleAlarmStateItem";
import {NetworkAccess} from "../APIAccess/NetworkAccess";

export const SitesPage = () => {
    const [sitesData, setSitesData] = useState<SingleAlarmStateItem[]>([]);
    const networkAccess = new NetworkAccess("http://localhost:3456")


    if (sitesData.length === 0)
        networkAccess.getSites(setSitesData).then(() => console.log("Sites fetch completed. "));

    return (
        <GridOfAlarmStateItems items={sitesData} redirectString={"/site/"}/>
    );
}
