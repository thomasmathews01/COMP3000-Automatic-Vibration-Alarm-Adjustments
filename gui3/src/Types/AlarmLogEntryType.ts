export interface AlarmLogEntry {
    siteName: string;
    machineName: string;
    channelName: string;
    typeName: string;
    time: Date;
    eventType: AlarmLogEventType;
}

export enum AlarmLogEventType {
    Activated,
    Deactivated
}

export const GetEventTypeString = (type: AlarmLogEventType) => {
    switch (type) {
        case AlarmLogEventType.Deactivated:
            return "Deactivated";
        case AlarmLogEventType.Activated:
            return "Activated";
    }

    return "";
}