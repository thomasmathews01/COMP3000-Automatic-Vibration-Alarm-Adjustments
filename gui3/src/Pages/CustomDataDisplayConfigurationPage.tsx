import React, {useState} from 'react';
import {Grid, MenuItem, Select, Slider, Typography} from "@material-ui/core";

function valuetext(value: number) {
    return `${value}°C`;
}

export const CustomDataDisplayConfigurationPage = () => {
    const [value, setValue] = useState<number[]>([20, 37]);

    const handleChange = (event: any, newValue: number | number[]) => {
        setValue(newValue as number[]);
    };

    return (
        <Grid container spacing={2}>
            <Grid item xs={12}>
                <Typography id="range-slider">
                    Custom Data Selection
                </Typography>
            </Grid>
            <Grid item xs={12}>
                <Grid container spacing={2}>
                    <Grid item xs={4}>
                        <Typography id="range-slider" gutterBottom>
                            Time range of data to display
                        </Typography>
                        <Slider
                            value={value}
                            onChange={handleChange}
                            valueLabelDisplay="auto"
                            aria-labelledby="range-slider"
                            getAriaValueText={valuetext}
                        />
                    </Grid>
                    <Grid item xs={8}>
                        <Grid container direction={"column"}>
                            <Grid item>
                                <Typography>
                                    Data Sources
                                </Typography>
                            </Grid>
                            <Grid item>
                                <Grid container direction={"row"}>
                                    <Grid item>
                                        <Select
                                            value={"Something"}
                                            onChange={(e) => console.log(e.target.value)}
                                            displayEmpty
                                            aria-label={"LABEL"}
                                            fullWidth={true}
                                        >
                                            <MenuItem value={"Something"}>Channel 1</MenuItem>
                                            <MenuItem value={"E1"}>Channel 2</MenuItem>
                                            <MenuItem value={"E2"}>Channel 3</MenuItem>
                                            <MenuItem value={"E3"}>Channel 4</MenuItem>
                                        </Select>
                                    </Grid>
                                    <Grid item>
                                        <Select
                                            value={"Something"}
                                            onChange={(e) => console.log(e.target.value)}
                                            displayEmpty
                                            aria-label={"LABEL"}
                                            fullWidth={true}
                                        >
                                            <MenuItem value={"Something"}>Channel 1</MenuItem>
                                            <MenuItem value={"E1"}>Channel 2</MenuItem>
                                            <MenuItem value={"E2"}>Channel 3</MenuItem>
                                            <MenuItem value={"E3"}>Channel 4</MenuItem>
                                        </Select>
                                    </Grid>
                                </Grid>
                            </Grid>
                        </Grid>
                    </Grid>
                </Grid>
            </Grid>
        </Grid>
    );
}

/*
    Breaking this down logically...
    We have a view consisting of a time selector and a data selector, and a decimation rate display
    and some sort of submit button
    then within that we have a list, that can be appended to or removed from, which will have to be its own small component
 */
