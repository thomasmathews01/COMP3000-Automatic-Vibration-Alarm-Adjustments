import {State} from "../Types/StateTypes";
import {Button, Grid, TextField, Typography} from "@material-ui/core";
import {useState} from "react";
import axios from "axios";

const addNewState = async (newStateName: string) => {
    try {
        const response = await axios.post(`http://localhost:3456/states?name=${newStateName}`, {});
        if (response.status !== 200)
            console.log("Error whilst adding a new state");
    } catch (exception) {
        console.log(`Exceptino whilst adding new state: ${exception.toString()}`);
    }
}

export const AddStateForm = (props: { states: State[] }) => {
    const [newStateName, updateNewStateName] = useState("");

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
                                onClick={() => {
                                    addNewState(newStateName);
                                    updateNewStateName("");
                                }}>Add</Button>
                    </Grid>
                </Grid>
            </Grid>
        </Grid>
    );
}