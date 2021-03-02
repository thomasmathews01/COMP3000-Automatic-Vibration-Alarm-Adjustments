import React from "react";
import {Grid, IconButton, Typography} from "@material-ui/core";
import {DataSource, DataSourceSelector} from "./DataSourceSelector";
import {AddCircle} from "@material-ui/icons";

export interface Props {
    dataSources: DataSource[],
    updateSources: (newSources: DataSource[]) => void
}

export const DataSources = (props: Props) => {
    const updateFn = (idx: number) => (src: DataSource | null) => {
        let oldSources = [...props.dataSources];
        if (src !== null)
            oldSources[idx] = src as DataSource;
        else
            oldSources.splice(idx, 1);

        props.updateSources(oldSources);
    }

    const getDataSourceEditor = (src: DataSource, index: number) => {
        return (
            <Grid item>
                <DataSourceSelector source={src} updateSourceFn={updateFn(index)}/>
            </Grid>
        );
    }

    const addDataSource = () => {
        let oldSources = props.dataSources;
        oldSources.push({site: -1, machine: -1, channel: -1, type: -1});
        props.updateSources([...oldSources]);
    }

    return (
        <Grid container direction={"column"} spacing={2}>
            <Grid item>
                <Typography>
                    Data Sources
                </Typography>
            </Grid>
            {
                props.dataSources.map(getDataSourceEditor)
            }
            <Grid item>
                <IconButton color="primary" component="span" onClick={addDataSource}>
                    <AddCircle/>
                </IconButton>
            </Grid>
        </Grid>
    );
}
