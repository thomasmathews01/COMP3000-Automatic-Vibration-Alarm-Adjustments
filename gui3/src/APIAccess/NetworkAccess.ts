import {Site} from "../Types/SiteTypes";
import {Machine} from "../Types/MachineTypes";
import {AlarmMode, AlarmSettings} from "../Types/AlarmSettingsTypes";
import {AlarmLogEntry, AlarmLogEventType} from "../Types/AlarmLogEntryType";
import axios from "axios";
import {alarmLevelChange, dataItem} from "../Types/DataTypes";
import {State, statePeriod} from "../Types/StateTypes";
import {SingleAlarmStateItem} from "../Types/SingleAlarmStateItem";
import {serverChannel, serverMachine, serverSite, type} from "./ServerTypes";

export class NetworkAccess {
    serverAddress: string;

    constructor(serverAddress?: string) {
        this.serverAddress = serverAddress ?? "${this.serverAddress}";
    }

    getAllSites(): Site[] {
        return [
            {id: 0, name: "Highbury"},
            {id: 1, name: "Northanger"}
        ];
    }

    getAllMachinesForSite(site: Site): Machine[] {
        return [
            {id: 0, name: site.name + " - Machine 1"},
            {id: 1, name: site.name + " - Machine 2"}
        ];
    }

    getAllAlarmSettingsForMachine(machine: Machine): AlarmSettings[] {
        const settings: AlarmSettings[] = [];
        const channels = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16];
        const types = ["RMS", "PkToPk", "Order 1"];

        channels.forEach(channelNumber => {
            types.forEach(typeString => {
                settings.push({
                    channelName: `Channel ${channelNumber}`,
                    typeName: typeString,
                    alarmMode: AlarmMode.StateBasedAuto
                })
            });
        });

        return settings;
    }

    UpdateAlarmSettingsForMachine(machine: Machine, updatedSettings: AlarmSettings): void {

    }

    GetAllAlarmLogEntries(): AlarmLogEntry[] {
        return [
            {siteName: 'Highbury', machineName: 'Machine 1', channelName: 'Channel 1', typeName: 'RMS', time: new Date(Date.now()), eventType: AlarmLogEventType.Activated},
            {siteName: 'Highbury', machineName: 'Machine 2', channelName: 'Channel 2', typeName: 'PkPk', time: new Date(Date.now()), eventType: AlarmLogEventType.Activated},
            {siteName: 'Northanger', machineName: 'Machine 1', channelName: 'Channel 1', typeName: 'Order 1', time: new Date(Date.now()), eventType: AlarmLogEventType.Deactivated},
            {siteName: 'Northanger', machineName: 'Machine 2', channelName: 'Channel 2', typeName: 'Subsync ', time: new Date(Date.now()), eventType: AlarmLogEventType.Deactivated}
        ];
    }


    async fetchAlarmChanges(channel: number, type: number, alarmType: string) {
        const response = await axios.get<{ alarms: alarmLevelChange[] }>(`${this.serverAddress}/${alarmType}s?channel=${channel}&type=${type}`);

        if (response.data.alarms) {
            console.log(`Retrieved ${response.data.alarms} alarm levels of type: ${alarmType}`);
            return response.data.alarms.map(item => {
                return {x: item.secondsSinceEpoch * 1000, y: item.value}
            });
        }

        return [];
    }

    async fetchValueData(channel: number, type: number) {
        const response = await axios.get<{ data: dataItem[] }>(`${this.serverAddress}/data?channel=${channel}&type=${type}`);
        if (response.data.data)
            return response.data.data.map(item => {
                return {x: item.secondsSinceEpoch * 1000, y: item.value}
            });

        return [];
    }

    async fetchStates() {
        const response = await axios.get<{ states: State[] }>(`${this.serverAddress}/states`);
        return response.data.states ?? [];
    }

    async networkFetchStatePeriods(channel: number) {
        const response = await axios.get<{ states: statePeriod[] }>(`${this.serverAddress}/statePeriods?channel=${channel}`);
        return response.data.states ?? [];
    }

    async deleteStateName(name: string) {
        const response = await axios.post(`${this.serverAddress}/deleteState?name=${name}`);

        if (response.status !== 200)
            console.log("Failed to issue state deletion request");
    }

    async fetchGraphTypes(): Promise<type[]> {
        try {
            const resp = await axios.get<{ types: type[] }>(`${this.serverAddress}/dataTypes?channel=${1}`);

            return resp.data.types;
        } catch (exception) {
            console.log(exception.toString());
            return [];
        }
    }

    async getChannels(machineID: number) {
        try {
            const resp = await axios.get<{ channels: serverChannel[] }>(`${this.serverAddress}/channels?machine=${machineID}`);

            return resp.data.channels.map((x) => {
                return {
                    id: x.id,
                    name: x.name,
                    state: 1
                }
            });
        } catch (exception) {
            console.log(exception.toString());
            return [];
        }
    };


    async getMachines(siteID: number) {
        try {
            const resp = await axios.get<{ machines: serverMachine[] }>(`${this.serverAddress}/machines?site=${siteID}`);

            return resp.data.machines.map((x) => {
                return {
                    id: x.id,
                    name: x.name,
                    state: 1
                }
            });
        } catch (exception) {
            console.log(exception.toString());
            return [];
        }
    };


    async getSites(updateFn: (x: SingleAlarmStateItem[]) => void) {
        try {
            const sites = await axios.get<{ sites: serverSite[] }>(`${this.serverAddress}/sites`);

            updateFn(sites.data.sites.map((x) => {
                return {
                    id: x.id,
                    name: x.name,
                    state: 1
                }
            }));
        } catch (exception) {
            console.log(exception.toString());
            return [];
        }
    };

    async addNewState(newStateName: string) {
        try {
            const response = await axios.post(`${this.serverAddress}/states?name=${newStateName}`, {});
            if (response.status !== 200)
                console.log("Error whilst adding a new state");
        } catch (exception) {
            console.log(`Exception whilst adding new state: ${exception.toString()}`);
        }
    }

    async issueStateUpdate(leftBound: number, rightBound: number, stateID: number) { // TODO: get this to work with multiple states.
        await axios.post(`${this.serverAddress}/stateUpdate?startTime=${leftBound}&endTime=${rightBound}&machine=${1}&stateId=${stateID}`)
    }
}