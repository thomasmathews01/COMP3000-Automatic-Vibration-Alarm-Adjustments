import React, {useState} from 'react';
import {Box, Grid, List, ListItem, ListItemText, MenuItem, Select, Typography} from "@material-ui/core";
import {NetworkAccess} from "../APIAccess/NetworkAccess";
import {AlarmMode, AlarmSettings, getAlarmModeString, getAllAlarmModes} from "../Types/AlarmSettingsTypes";
import {Machine} from "../Types/MachineTypes";

const AlarmSettingItem = (props: { information: AlarmSettings, updateSetting: (newValues: AlarmSettings) => void }) => {
    return (
        <Grid container direction="row">
            <Grid item xs={4} md={4}>
                <Typography>{props.information.channelName + " - " + props.information.typeName}</Typography>
            </Grid>
            <Grid item xs={4} md={4}>
                <Select onChange={(event) => {
                    const newMode = event.target.value as AlarmMode;
                    const newInformation: AlarmSettings = {...props.information, ...{alarmMode: newMode}};

                    props.updateSetting(newInformation);
                }} value={props.information.alarmMode}>
                    {getAllAlarmModes().map((option: AlarmMode) => (
                        <MenuItem value={option}>{getAlarmModeString(option)}</MenuItem>
                    ))}
                </Select>
            </Grid>
        </Grid>
    );
}

export const AlarmSettingsPage = () => {
    const [selectedSiteIndex, setSelectedSiteIndex] = useState<number | undefined>(undefined);
    const [selectedMachineIndex, setSelectedMachineIndex] = useState<number | undefined>(undefined);
    const networkAccess = new NetworkAccess();
    const sites = networkAccess.getAllSites();
    const [machines, setMachines] = useState<Machine[]>([]);
    const [alarmSettings, setAlarmSettings] = useState<AlarmSettings[]>([]);

    const updateCurrentSiteIndex = (newIndex: number) => {
        if (newIndex === selectedSiteIndex)
            return; // Nothing to do.

        setSelectedSiteIndex(newIndex);
        setSelectedMachineIndex(undefined); // This can't still be valid, since the machine doesn't belong to this site;
        setMachines(networkAccess.getAllMachinesForSite(sites[newIndex]));
        setAlarmSettings([]);
    }

    const updateCurrentMachineIndex = async (newIndex: number) => {
        if (newIndex === selectedMachineIndex)
            return; // Nothing to do.

        setSelectedMachineIndex(newIndex);
        setAlarmSettings(await networkAccess.getAllAlarmSettingsForMachine(machines[newIndex]));
    }

    return (
        <Grid container direction="row">
            <Grid item xs={4} md={4}>
                <List>
                    {
                        sites.map((site, index) => (
                            <ListItem button selected={selectedSiteIndex === index} onClick={() => updateCurrentSiteIndex(index)}>
                                <ListItemText primary={site.name}/>
                            </ListItem>
                        ))
                    }
                </List>
            </Grid>
            <Grid item xs={4} md={4}>
                {
                    machines.map((machine, index) => (
                        <ListItem button selected={selectedMachineIndex === index} onClick={async () => await updateCurrentMachineIndex(index)}>
                            <ListItemText primary={machine.name}/>
                        </ListItem>
                    ))
                }
            </Grid>
            <Grid item xs={4} md={4}>
                <Box overflow={"auto"}>
                    <List>
                        {alarmSettings.map(settings => (
                            <ListItem>
                                <AlarmSettingItem information={settings} updateSetting={(newSettings: AlarmSettings) => {
                                    networkAccess.UpdateAlarmSettingsForMachine(machines[selectedMachineIndex ?? 0], newSettings);
                                }}/>
                            </ListItem>
                        ))}
                    </List>
                </Box>
            </Grid>
        </Grid>
    );
}
