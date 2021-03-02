import {Site} from "../Types/SiteTypes";
import {Machine} from "../Types/MachineTypes";
import {AlarmMode, AlarmSettings} from "../Types/AlarmSettingsTypes";
import {AlarmLogEntry, AlarmLogEventType} from "../Types/AlarmLogEntryType";
import axios from "axios";
import {alarmLevelChange} from "../Types/DataTypes";
import {State, statePeriod} from "../Types/StateTypes";
import {SingleAlarmStateItem} from "../Types/SingleAlarmStateItem";
import {serverChannel, serverMachine, serverSite, serverType} from "./ServerTypes";

export class NetworkAccess {
    serverAddress: string;

    constructor(serverAddress?: string) {
        this.serverAddress = serverAddress ?? `http://localhost:1234`;
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
        const response = await axios.get<{ times: number[], values: number[] }>(`${this.serverAddress}/data?channel=${channel}&type=${type}&start=946728000&end=1614011510`);
        if (response.data)
            return response.data.times.map((time, idx) => {
                return {x: time * 1000, y: response.data.values[idx]}; //TODO: Vulnerable to bad array access
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
        const response = await axios.delete(`${this.serverAddress}/states?name=${name}`);

        if (response.status !== 200)
            console.log("Failed to issue state deletion request");
    }

    async fetchGraphTypes(): Promise<serverType[]> {
        try {
            const resp = await axios.get<{ types: serverType[] }>(`${this.serverAddress}/dataTypes?channel=${1}`); // TODO: Get using the channel ...

            return resp.data.types;
        } catch (exception) {
            console.log(exception.toString());
            return [];
        }
    }

    async getChannels(machineID: number) {
        try {
            const resp = await axios.get<{ channels: serverChannel[] }>(`${this.serverAddress}/channels?machine=${machineID}`);

            return resp.data.channels;
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


    async getSites(): Promise<SingleAlarmStateItem[]> {
        try {
            const sites = await axios.get<{ sites: serverSite[] }>(`${this.serverAddress}/sites`);

            return sites.data.sites.map((x) => {
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

    async getEarliestDataTime() { // TODO: get this to work with multiple states.
        const response = await axios.get<{ secondsSinceEpoch: number }>(`${this.serverAddress}/earliestDataTime?machine=1`)
        return response.data.secondsSinceEpoch;
    }

    async getLatestDataTime() { // TODO: get this to work with multiple states.
        const response = await axios.get<{ secondsSinceEpoch: number }>(`${this.serverAddress}/latestDataTime?machine=1`)
        return response.data.secondsSinceEpoch
    }

    async getChannelInformationFromID(id: number) {
        const response = await axios.get<{ name: string, id: number }>(`${this.serverAddress}/channelInformation?channel_id=${id}`);
        return response.data;
    }

    async getTypeInformationFromID(id: number) {
        const response = await axios.get<{ name: string, id: number }>(`${this.serverAddress}/typeInformation?type_id=${id}`);
        return response.data;
    }

    async getCurrentState(query: string) {
        const response = await axios.get<{ state: string }>(`${this.serverAddress}/currentAlarmState?${query}`);
        return response.data;
    }

    async login(username: string, password: string) {
        // TODO: We probably ought to encrypt this somehow.
        const response = await axios.post(`${this.serverAddress}/login?username=${username}&password=${password}`, {});
        return response && response.data === "Accepted";
    }
}