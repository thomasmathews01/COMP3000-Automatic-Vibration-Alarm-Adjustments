import React, {useState} from 'react';
import {State} from "../Types/StateTypes";
import {Grid, MenuItem, Select, Typography} from "@material-ui/core";
import {StatesGrid} from "../Components/StatesGrid";
import {AddStateForm} from "../Components/AddStatesForm";
import {NetworkAccess} from "../APIAccess/NetworkAccess";

export const StateSettingsPage = () => {
    const [states, setStates] = useState<State[]>([]);
    const networkAccess = new NetworkAccess();

    if (states?.length === 0)
        networkAccess.fetchStates().then(newStates => setStates(newStates));

    return (
        <Grid container direction="column" spacing={2}>
            <Grid item>
                <Grid container direction={"row"}>
                    <Grid item>
                        <Typography>Site</Typography>
                    </Grid>
                    <Grid item>
                        <Select>
                            <MenuItem>Something</MenuItem>
                            <MenuItem>Something Else</MenuItem>
                            <MenuItem>Something Else Entirely</MenuItem>
                        </Select>
                    </Grid>
                    <Grid item>
                        <Typography>Machine</Typography>
                    </Grid>
                    <Grid item>
                        <Select>
                            <MenuItem>Something</MenuItem>
                            <MenuItem>Something Else</MenuItem>
                            <MenuItem>Something Else Entirely</MenuItem>
                        </Select>
                    </Grid>
                </Grid>
            </Grid>
            <Grid item>
                <StatesGrid states={states}/>
            </Grid>
            <Grid item>
                <AddStateForm states={states}/>
            </Grid>
        </Grid>
    );
}
