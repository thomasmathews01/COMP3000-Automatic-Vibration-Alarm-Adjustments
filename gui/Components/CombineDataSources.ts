import {alarmLevelChange, dataItem} from '../Types/DataTypes'

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
    const levels = await Promise.all([getLevels(data, alerts), getLevels(data, alarms)]);

    return data.map((point, index) => {
        return {...point, ...{alarmLevel: levels[1][index], alertLevel: levels[0][index]}}
    });
}

export const getLevels = async (data: dataItem[], alarms: alarmLevelChange[]): Promise<number[]> => {
    alarms = alarms.filter(y => data.some(x => x.secondsSinceEpoch >= y.secondsSinceEpoch)); // Remove nonsense values

    if (alarms.length == 0) return [];

    const pivotIndices = alarms.map(alarm => data.findIndex(dataPoint => dataPoint.secondsSinceEpoch >= alarm.secondsSinceEpoch));

    const getAllPointsBeforeNextPivot = (pivot: number, indexOfPivot: number) => {
        const nextPivot = pivotIndices[indexOfPivot + 1] ?? data.length; // If there is no next pivot, take all values until the end.
        const countOfPointsDuringAlarmPeriod = nextPivot - pivot;
        return new Array(countOfPointsDuringAlarmPeriod).fill(alarms[indexOfPivot].level);
    }

    return pivotIndices.map(getAllPointsBeforeNextPivot).reduce((x, y) => x.concat(y), []);
}
