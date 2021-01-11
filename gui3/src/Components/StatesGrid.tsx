import React, {useState} from 'react';
import {ColDef, DataGrid} from '@material-ui/data-grid';
import {State} from "../Types/StateTypes";
import {Button, Grid} from "@material-ui/core";
import axios from "axios";

const columns: ColDef[] = [
    {field: 'id', headerName: 'State ID', width: 150},
    {field: 'name', headerName: 'State Name', width: 150}
];

const deleteStateName = async (name: string) => {
    const response = await axios.post(`http://localhost:3456/deleteState?name=${name}`);

    if (response.status !== 200)
        console.log("Failed to issue state deletion request");
}

export const StatesGrid = (props: { states: State[] }) => {
    const [selectedRows, updateSelectedRows] = useState<number[]>([]);

    const deleteSelectedRows = async () => {
        if (!selectedRows.includes(0))
            selectedRows.map(x => props.states[x].name).forEach(stateName => deleteStateName(stateName));
        else console.log("Cannot remove the original unknown state, must always be present.");
    }

    if (props.states?.length > 0)
        return (
            <Grid container direction="column">
                <Grid item>
                    <div style={{height: 400, width: 350}}>
                        <DataGrid rows={props.states} columns={columns} pageSize={10} checkboxSelection onSelectionChange={(selected) =>
                            updateSelectedRows(selected.rowIds.map(x => parseInt(x.toString()) - 1))
                        }/>
                    </div>
                </Grid>
                <Grid item>
                    <Button disabled={selectedRows.length < 1} onClick={deleteSelectedRows}>Delete Selected</Button>
                </Grid>
            </Grid>

        );

    return (<div></div>);
}
