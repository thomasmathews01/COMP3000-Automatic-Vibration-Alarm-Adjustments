export enum AlarmMode {
    StateBasedAuto,
    AutomaticBasic,
    FixedISOClassA,
    FixedISOClassB,
    None
}

export const getAlarmModeString = (mode: AlarmMode) => {
    switch (mode) {
        case AlarmMode.StateBasedAuto:
            return "Automatic - State Based";
        case AlarmMode.AutomaticBasic:
            return "Automatic - Basic";
        case AlarmMode.FixedISOClassA:
            return "Fixed - ISO Class A";
        case AlarmMode.FixedISOClassB:
            return "Fixed - ISO Class B";
        case AlarmMode.None:
            return "None";
    }

    throw new Error("Alarm mode not recognised");
}

export const getAllAlarmModes = (): AlarmMode[] => {
    return [
        AlarmMode.StateBasedAuto,
        AlarmMode.AutomaticBasic,
        AlarmMode.FixedISOClassA,
        AlarmMode.FixedISOClassB,
        AlarmMode.None
    ];
}

export interface AlarmSettings {
    channelName: string,
    typeName: string,
    alarmMode: AlarmMode
}