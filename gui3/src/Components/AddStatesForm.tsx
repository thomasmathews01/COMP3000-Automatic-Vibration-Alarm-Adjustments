import React, {useState} from 'react';
import {State} from "../Types/StateTypes";
import {Button, Grid, TextField, Typography} from "@material-ui/core";
import {NetworkAccess} from "../APIAccess/NetworkAccess";

export const AddStateForm = (props: { states: State[] }) => {
    const [newStateName, updateNewStateName] = useState("");
    const networkAccess = new NetworkAccess();

    const newNameIsUnique = (): boolean => {
        return props.states.find(state => state.name.toLowerCase() === newStateName.toLowerCase()) === undefined;
    }

    return (
        <Grid container direction="column">
            <Grid item>
                <Typography>Add New State</Typography>
            </Grid>
            <Grid item>
                <Grid container direction="row">
                    <Grid item>
                        <TextField value={newStateName} onChange={(updateEvent) => {
                            console.log(updateEvent.target.value);
                            updateNewStateName(updateEvent.target.value);
                        }}/>
                    </Grid>
                    <Grid item>
                        <Button disabled={newStateName === "" || !newNameIsUnique()}
                                variant={"contained"}
                                onClick={async () => {
                                    await networkAccess.addNewState(newStateName);
                                    updateNewStateName("");
                                }}>Add</Button>
                    </Grid>
                </Grid>
            </Grid>
        </Grid>
    );
}
