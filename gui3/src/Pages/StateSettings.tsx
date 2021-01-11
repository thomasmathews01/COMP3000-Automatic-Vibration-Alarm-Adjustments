import React, {useState} from 'react';
import axios from "axios";
import {State} from "../Types/StateTypes";
import {Grid} from "@material-ui/core";
import {StatesGrid} from "../Components/StatesGrid";
import {AddStateForm} from "../Components/AddStatesForm";

const getStates = async (): Promise<State[]> => {
    try {
        const response = await axios.get<{ states: State[] }>(`http://localhost:3456/states`);

        return response.data.states;
    } catch (exception) {
        console.log(`Error retrieving states: ${exception.toString()}`);
    }

    return [];
};

export const StateSettingsPage = () => {
    const [states, setStates] = useState<State[]>([]);

    if (states?.length === 0)
        getStates().then(newStates => setStates(newStates));

    return (
        <Grid container direction="column">
            <Grid item>
                <div>
                    <StatesGrid states={states}/>
                </div>
            </Grid>
            <Grid item>
                <div>
                    <AddStateForm states={states}/>
                </div>
            </Grid>
        </Grid>
    );
}
