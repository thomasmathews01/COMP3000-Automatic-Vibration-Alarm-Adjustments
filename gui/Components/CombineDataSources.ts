import {alarmLevelChange, dataItem} from '../Types/DataTypes'

/*
    We have an expectation that the request time of the alarm changes will contain an entry at time = 0 with the level at that moment in time.
 */

export const addStateData = (data: dataItem[]) => {
    if (!data || data.length < 2)
        return data;

    const maxValue = Math.max(...data.map(x => x.value));

    return data.map(x => {
        x.states = [];
        x.states.push({name: "Unknown", value: maxValue});
        return x;
    });
};

export const addAlarmData = async (data: dataItem[], alarms: alarmLevelChange[], alerts: alarmLevelChange[]) => {
    const alertLevels = await getLevels(data, alerts);
    const alarmLevels = await getLevels(data, alarms);

    return data.map((point, index) => {
        return {...point, ...{alarmLevel: alarmLevels[index], alertLevel: alertLevels[index]}}
    });
}

export const getLevels = async (data: dataItem[], alarms: alarmLevelChange[], levels?: number[]): Promise<number[]> => {
    if (alarms.length == 0) return levels ?? [];
}
