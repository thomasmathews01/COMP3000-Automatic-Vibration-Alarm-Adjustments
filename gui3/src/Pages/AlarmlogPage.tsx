import React from 'react';
import {ColDef, DataGrid} from '@material-ui/data-grid';
import {NetworkAccess} from "../APIAccess/NetworkAccess";

const columns: ColDef[] = [
    {field: 'siteName', headerName: 'Site Name', width: 150},
    {field: 'machineName', headerName: 'Machine Name', width: 150},
    {field: 'channelName', headerName: 'Channel Name', width: 150},
    {field: 'typeName', headerName: 'Type Name', width: 150},
    {field: 'time', headerName: 'Event Time', width: 150},
    {field: 'eventType', headerName: 'Activation/Deactivation', width: 250}
];

export const AlarmLogPage = () => {
    const networkAccess = new NetworkAccess('http://localhost:3456');
    const alarmLogEntries = networkAccess.GetAllAlarmLogEntries();
    const rows = alarmLogEntries.map((entry, index) => {
        return {...entry, ...{id: index}}; // ID isn't the ID of the entry itself, just a requirement of the data grid itself.
    });

    return (
        <div style={{height: 400, width: '100%'}}>
            <DataGrid rows={rows} columns={columns} pageSize={500} checkboxSelection/>
        </div>
    );
}
