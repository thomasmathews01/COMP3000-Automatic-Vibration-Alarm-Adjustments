import React, {useState} from "react";
import {Grid, IconButton, Typography} from "@material-ui/core";
import {DataSource, DataSourceSelector} from "./DataSourceSelector";
import {NetworkAccess} from "../APIAccess/NetworkAccess";
import {serverChannel, serverType} from "../APIAccess/ServerTypes";
import {AddCircle} from "@material-ui/icons";

const getServerChannels = async () => {
    const network = new NetworkAccess();
    return network.getChannels(1); // TODO: Sane machine policy
}

const getServerTypes = async () => {
    const network = new NetworkAccess();
    return network.fetchGraphTypes();
}

export interface Props {
    dataSources: DataSource[],
    updateSources: (newSources: DataSource[]) => void
}

export const DataSources = (props: Props) => {
    const [allChannels, setAllChannels] = useState<serverChannel[]>([]);
    if (allChannels.length === 0)
        getServerChannels().then(channels => {
            setAllChannels(channels);
            let sources = [...props.dataSources];
            sources[0].channel = channels[0].id;
            props.updateSources(sources);
        });

    const [allTypes, setAllTypes] = useState<serverType[]>([]);
    if (allTypes.length === 0)
        getServerTypes().then(types => {
            setAllTypes(types);
            let sources = [...props.dataSources];
            sources[0].type = types[0].id;
            props.updateSources(sources);
        });

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
                <DataSourceSelector source={src} updateSourceFn={updateFn(index)} allChannels={allChannels} allTypes={allTypes}/>
            </Grid>
        );
    }

    const addDataSource = () => {
        let oldSources = props.dataSources;
        oldSources.push({channel: allChannels[0].id, type: allTypes[0].id});
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
