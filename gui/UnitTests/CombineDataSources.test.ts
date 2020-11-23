import {expect} from 'chai'
import {dataItem} from "../Types/DataTypes";
import {addAlarmData} from "../Components/CombineDataSources";

describe('DataFetching', () => {
    it('Unit Tests can run', function () {
        expect(1).to.eq(1);
    });

    it('Returns data un-modified if there are no alarms/alerts', async function () {
        const data: dataItem[] = [
            {secondsSinceEpoch: 0, value: 100},
            {secondsSinceEpoch: 1, value: 100},
            {secondsSinceEpoch: 2, value: 100},
            {secondsSinceEpoch: 3, value: 100},
        ]

        const result = await addAlarmData(data, [], []);

        expect(result.length).to.eq(data.length);
        result.forEach(x => {
            expect(x.alarmLevel).to.be.undefined;
            expect(x.alertLevel).to.be.undefined;
        })
    });

    it('Returns handles single alarm', async function () {
        const data: dataItem[] = [
            {secondsSinceEpoch: 1, value: 100},
            {secondsSinceEpoch: 2, value: 100},
            {secondsSinceEpoch: 3, value: 100},
            {secondsSinceEpoch: 4, value: 100},
        ]

        const result = await addAlarmData(data, [{secondsSinceEpoch: 1, level: 100}], []);

        expect(result.length).to.eq(data.length);
        result.forEach(x => {
            expect(x.alarmLevel).to.eq(100);
            expect(x.alertLevel).to.be.undefined;
        })
    });

    it('Returns handles multiple alarms', async function () {
        const data: dataItem[] = [
            {secondsSinceEpoch: 1, value: 100},
            {secondsSinceEpoch: 2, value: 100},
            {secondsSinceEpoch: 3, value: 100},
            {secondsSinceEpoch: 4, value: 100},
        ]

        const result = await addAlarmData(data, [{secondsSinceEpoch: 1, level: 100}, {secondsSinceEpoch: 3, level: 1000}], []);

        expect(result.length).to.eq(data.length);
        expect(result.filter(x => x.alarmLevel === 100)).to.eq(2);
        expect(result.filter(x => x.alarmLevel === 1000)).to.eq(2);
    });
});
