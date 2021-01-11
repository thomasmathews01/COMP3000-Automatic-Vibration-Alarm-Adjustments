import {Site} from "../Types/SiteTypes";
import {Machine} from "../Types/MachineTypes";
import {AlarmMode, AlarmSettings} from "../Types/AlarmSettingsTypes";
import {AlarmLogEntry, AlarmLogEventType} from "../Types/AlarmLogEntryType";

export class NetworkAccess {
    serverAddress: string;

    constructor(serverAddress: string) {
        this.serverAddress = serverAddress;
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
}