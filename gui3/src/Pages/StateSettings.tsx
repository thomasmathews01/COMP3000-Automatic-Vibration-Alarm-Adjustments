import React, {useState} from 'react';
import {State} from "../Types/StateTypes";
import {Grid} from "@material-ui/core";
import {StatesGrid} from "../Components/StatesGrid";
import {AddStateForm} from "../Components/AddStatesForm";
import {NetworkAccess} from "../APIAccess/NetworkAccess";

export const StateSettingsPage = () => {
    const [states, setStates] = useState<State[]>([]);
    const networkAccess = new NetworkAccess();

    if (states?.length === 0)
        networkAccess.fetchStates().then(newStates => setStates(newStates));

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
