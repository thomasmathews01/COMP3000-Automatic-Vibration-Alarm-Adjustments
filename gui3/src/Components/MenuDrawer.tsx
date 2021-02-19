import {Drawer, List, ListItem, ListItemIcon, ListItemText} from "@material-ui/core";
import React from "react";
import AlarmIcon from '@material-ui/icons/Alarm';
import BrokenImageIcon from '@material-ui/icons/BrokenImage';
import {useHistory} from "react-router-dom";

export const MenuDrawer = (props: { open: boolean, setOpen: (newState: boolean) => void }) => {
    const history = useHistory();

    const routeToPage = async (redirectString: string) => {
        console.log(`redirecting to: ${redirectString}`);
        history.push(redirectString);
        props.setOpen(false);
    }

    return (
        <Drawer anchor='left' open={props.open} onClose={() => props.setOpen(false)}>
            <List>
                <ListItem button key={"Live States Display"} onClick={() => routeToPage("/sites")}>
                    <ListItemIcon><BrokenImageIcon/></ListItemIcon>
                    <ListItemText primary={"Live States Display"}/>
                </ListItem>
                <ListItem button key={"Custom Data Display"} onClick={() => routeToPage("/customDataDisplayConfiguration")}>
                    <ListItemIcon><BrokenImageIcon/></ListItemIcon>
                    <ListItemText primary={"Custom Data Display"}/>
                </ListItem>
                <ListItem button key={"State Settings"} onClick={() => routeToPage("/statesettings/1")}>
                    <ListItemIcon><AlarmIcon/></ListItemIcon>
                    <ListItemText primary={"State Settings"}/>
                </ListItem>
                <ListItem button key={"Alarm Settings"} onClick={() => routeToPage("/alarmsettings/1")}>
                    <ListItemIcon><AlarmIcon/></ListItemIcon>
                    <ListItemText primary={"Alarm Settings"}/>
                </ListItem>
                <ListItem button key={"Alarm Log"} onClick={() => routeToPage("/alarmlog/1")}>
                    <ListItemIcon><AlarmIcon/></ListItemIcon>
                    <ListItemText primary={"Alarm Log"}/>
                </ListItem>
            </List>
        </Drawer>
    );
}