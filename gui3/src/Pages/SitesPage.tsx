import React, {useState} from 'react';
import GridOfAlarmStateItems from "../Components/GridOfAlarmStateItems";
import {SingleAlarmStateItem} from "../Types/SingleAlarmStateItem";
import {NetworkAccess} from "../APIAccess/NetworkAccess";
import {useAsync} from 'react-async-hook';

const getSites = async () => {
    const networkAccess = new NetworkAccess();
    return await networkAccess.getSites();
}

export const SitesPage = () => {
    const [sitesData, setSitesData] = useState<SingleAlarmStateItem[]>([]);
    const asyncSites = useAsync(getSites, []);

    if (sitesData.length === 0 && !asyncSites.loading && !asyncSites.error && asyncSites.result)
        setSitesData(asyncSites.result);

    return (
        <GridOfAlarmStateItems items={sitesData} redirectString={"/site/"} fetchStringGenerator={(item: SingleAlarmStateItem) => `site_id=${item.id}`}/>
    );
}
