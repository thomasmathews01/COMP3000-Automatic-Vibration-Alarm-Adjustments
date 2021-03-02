import React, {useState} from "react";
import {Grid, IconButton} from "@material-ui/core";
import {ChannelSelector} from "./ChannelSelector";
import {TypeSelector} from "./TypeSelector";
import {serverChannel, serverMachine, serverSite, serverType} from "../APIAccess/ServerTypes";
import {RemoveCircle} from "@material-ui/icons";
import {NetworkAccess} from "../APIAccess/NetworkAccess";

const getServerChannels = async (machine: number) => {
    const network = new NetworkAccess();
    return network.getChannels(machine);
}

const getServerMachines = async (site: number) => {
    const network = new NetworkAccess();
    return network.getMachines(site);
}

const getServerSites = async () => {
    const network = new NetworkAccess();
    return network.getBasicSiteInformation();
}

const getServerTypes = async () => {
    const network = new NetworkAccess();
    return network.fetchGraphTypes();
}

export interface DataSource {
    site: number;
    machine: number;
    channel: number;
    type: number;
}

export interface Props {
    updateSourceFn: (newSource: DataSource | null) => void,
    source: DataSource
}

export const DataSourceSelector = (props: Props) => {
    const [allSites, setAllSites] = useState<serverSite[]>([]);
    const [allMachines, setAllMachines] = useState<serverMachine[]>([]);
    const [allChannels, setAllChannels] = useState<serverChannel[]>([]);
    const [allTypes, setAllTypes] = useState<serverType[]>([]);

    if (allSites.length === 0)
        getServerSites().then(sites => {
            setAllSites(sites);
            updateSite(sites[0].id);
        });

    if (allTypes.length === 0)
        getServerTypes().then(types => {
            setAllTypes(types);
            updateType(types[0].id);
        });

    const updateChannel = (newChannel: number) =>
        props.updateSourceFn({...props.source, ...{channel: newChannel}});

    const updateType = (newType: number) =>
        props.updateSourceFn({...props.source, ...{type: newType}});

    const updateSite = async (newSite: number) => {
        const machines = await getServerMachines(newSite);
        setAllMachines(machines);
        updateMachine(machines[0].id);

        props.updateSourceFn({...props.source, ...{site: newSite}});
    }

    const updateMachine = async (newMachine: number) => {
        const channels = await getServerChannels(newMachine);
        setAllChannels(channels);
        updateChannel(channels[0].id);
        props.updateSourceFn({...props.source, ...{machine: newMachine}});
    }

    return (
        <Grid container direction={"row"} spacing={2}>
            <Grid item>
                <TypeSelector types={allSites} selectedType={props.source.site} updateFn={updateSite}/>
            </Grid>
            <Grid item>
                <TypeSelector types={allMachines} selectedType={props.source.machine} updateFn={updateMachine}/>
            </Grid>
            <Grid item>
                <ChannelSelector channels={allChannels} selectedChannel={props.source.channel} updateFn={updateChannel}/>
            </Grid>
            <Grid item>
                <TypeSelector types={allTypes} selectedType={props.source.type} updateFn={updateType}/>
            </Grid>
            <Grid item>
                <IconButton color="primary" component="span" onClick={() => props.updateSourceFn(null)}>
                    <RemoveCircle/>
                </IconButton>
            </Grid>
        </Grid>
    );
}