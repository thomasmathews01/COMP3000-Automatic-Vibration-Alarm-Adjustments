export interface stateValue {
    name: string,
    value: number
}

export interface dataItem {
    secondsSinceEpoch: number,
    value: number,
    alarmLevel?: number,
    alertLevel?: number,
    states?: stateValue[]
}

export interface alarmLevelChange {
    secondsSinceEpoch: number,
    value: number,
}