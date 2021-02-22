import {DataSource} from "../Components/DataSourceSelector";

export interface CustomDataSet {
    dataSources: DataSource[];
    startTimeEpochSeconds: number;
    endTimeEpochSeconds: number;
}