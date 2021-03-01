import React, {useState} from 'react';
import {SingleAlarmStateItem} from "../Types/SingleAlarmStateItem"
import {makeStyles} from "@material-ui/core/styles";
import {Button, Container} from "@material-ui/core";
import {useHistory} from "react-router-dom";
import {NetworkAccess} from "../APIAccess/NetworkAccess";
import {useAsync} from 'react-async-hook';

const useStyles = makeStyles((theme) => ({
    button1: {
        minHeight: "40vh",
        minWidth: "40vh",
        background: theme.palette.primary.main,
    },
    button2: {
        minHeight: "40vh",
        minWidth: "40vh",
        background: theme.palette.warning.main,
    },
    button3: {
        minHeight: "40vh",
        minWidth: "40vh",
        background: theme.palette.error.main,
    }
}));

const getCurrentState = async (fetchString: string) => {
    const netAccess = new NetworkAccess();
    return (await netAccess.getCurrentState(fetchString)).state;
}

const AlarmStateItem = (props: { item: SingleAlarmStateItem, redirectString: string, stateFetchString: string }) => {
    const classes = useStyles();
    const history = useHistory();
    const [state, setState] = useState<string | undefined>(undefined);

    const currentState = useAsync(getCurrentState, [props.stateFetchString]);
    if (state === undefined && !currentState.loading && !currentState.error && currentState.result)
        setState(currentState.result);

    const routeToPage = async () => history.push(props.redirectString + props.item.id.toString());

    return (
        <Container>
            <Button className={state === "none" ? classes.button1 : state === "alert" ? classes.button2 : classes.button3} onClick={routeToPage}>
                {`${props.item.name}`}
            </Button>
        </Container>
    );
};

export default AlarmStateItem;