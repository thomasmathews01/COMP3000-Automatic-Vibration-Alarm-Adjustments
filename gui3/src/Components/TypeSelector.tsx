import {MenuItem, Select} from "@material-ui/core";
import React from "react";
import {serverType} from "../APIAccess/ServerTypes";

export interface Props {
    updateFn: (newTypeID: number) => void,
    selectedType: number,
    types: serverType[]
}

const getMenuItemFromType = (type: serverType) => {
    return (
        <MenuItem value={type.id}>{type.name}</MenuItem>
    );
}

export const TypeSelector = (props: Props) => {
    return (
        <Select
            value={props.selectedType}
            onChange={(e) => props.updateFn(e.target.value as number)}
            displayEmpty
            aria-label={"LABEL"}
            fullWidth={true}
        >
            {
                props.types.map(getMenuItemFromType)
            }
        </Select>
    );
}