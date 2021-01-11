import React, {FunctionComponent, useState} from 'react';
import {makeStyles} from '@material-ui/core/styles';
import AppBar from '@material-ui/core/AppBar';
import Toolbar from '@material-ui/core/Toolbar';
import Typography from '@material-ui/core/Typography';
import IconButton from '@material-ui/core/IconButton';
import MenuIcon from '@material-ui/icons/Menu';
import {Container} from "@material-ui/core";
import {MenuDrawer} from "./MenuDrawer";

const useStyles = makeStyles((theme) => ({
    root: {
        flexGrow: 1,
    },
    menuButton: {
        marginRight: theme.spacing(2),
    },
    title: {
        flexGrow: 1,
    },
    navBar: {
        marginBottom: 10
    }
}));

export const OuterPage: FunctionComponent = ({children}) => {
    const classes = useStyles();
    const [menuDrawerOpen, setMenuDrawerOpen] = useState(false);

    return (
        <div>
            <AppBar position="static" className={classes.navBar}>
                <Toolbar>
                    <IconButton edge="start" className={classes.menuButton} color="inherit" aria-label="menu" onClick={() => setMenuDrawerOpen(true)}>
                        <MenuIcon/>
                    </IconButton>
                    <Typography variant="h6" className={classes.title}>
                        Home
                    </Typography>
                </Toolbar>
            </AppBar>
            <Container fixed maxWidth="xl">
                <div>
                    <MenuDrawer open={menuDrawerOpen} setOpen={setMenuDrawerOpen}/>
                    {children}
                </div>
            </Container>
        </div>
    );
};
