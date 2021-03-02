import React, {useState} from 'react';
import {DataSelection} from "../Components/DataSelection";
import {CustomDataSet} from "../Types/CustomDataSet";
import {CustomDataView} from "../Components/CustomDataView";

export const CustomDataDisplayConfigurationPage = () => {
    const [settingData, setSettingData] = useState(true);
    const [dataView, setDataView] = useState<CustomDataSet>({
        startTimeEpochSeconds: 0,
        endTimeEpochSeconds: 100,
        dataSources: [
            {site: 1, machine: 1, channel: 1, type: 1}
        ]
    });

    return (
        <div>
            {
                settingData ? <DataSelection finishedSettingData={() => setSettingData(false)} dataDescription={dataView} updateDataDescription={setDataView}/>
                    : <CustomDataView dataView={dataView} reConfigureFn={() => setSettingData(true)}/>
            }
        </div>
    );
}
