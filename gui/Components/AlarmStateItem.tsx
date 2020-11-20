import React from 'react';
import {SingleAlarmStateItem} from "../Types/AlarmStateItem"
import {makeStyles} from "@material-ui/core/styles";
import {Button, Container} from "@material-ui/core";
import {useRouter} from 'next/router'

const useStyles = makeStyles((theme) => ({
    button1: {
        minHeight: "40vh",
        minWidth: "40vh",
        background: theme.palette.primary.main,
    },
    button2: {
        minHeight: "40vh",
        minWidth: "40vh",
        background: theme.palette.secondary.main,
    }
}));


const AlarmStateItem = (props: { item: SingleAlarmStateItem, redirectString: string }) => {
    const classes = useStyles();
    const router = useRouter();

    const routeToPage = async (redirectString: string, id: number) => {
        await router.push(redirectString + id.toString());
    }

    return (
        <Container>
            <Button className={props.item.state == 1 ? classes.button1 : classes.button2}
                    onClick={() => routeToPage(props.redirectString, props.item.id)}>
                {props.item.name}
            </Button>
        </Container>
    );
};

export default AlarmStateItem;