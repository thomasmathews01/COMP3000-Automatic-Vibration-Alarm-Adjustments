export enum AlarmState {
    InAlarm,
    NotInAlarm,
    Unknown
}

export interface SingleAlarmStateItem {
    id: number,
    name: string,
    state: AlarmState;
}
