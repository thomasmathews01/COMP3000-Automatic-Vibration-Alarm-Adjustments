export interface State {
    id: number,
    name: string
}

export interface StateValue {
    id: number,
    value: number
}

export interface statePeriod {
    id: number,
    start_seconds_since_epoch: number,
    end_seconds_since_epoch: number
}