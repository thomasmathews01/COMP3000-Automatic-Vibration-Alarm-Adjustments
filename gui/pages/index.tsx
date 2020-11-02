import React from 'react';
import Avatar from '@material-ui/core/Avatar';
import Button from '@material-ui/core/Button';
import CssBaseline from '@material-ui/core/CssBaseline';
import TextField from '@material-ui/core/TextField';
import Box from '@material-ui/core/Box';
import LockOutlinedIcon from '@material-ui/icons/LockOutlined';
import Typography from '@material-ui/core/Typography';
import {makeStyles} from '@material-ui/core/styles';
import Container from '@material-ui/core/Container';
import axios from 'axios'
import {useRouter} from 'next/router'
import {Snackbar} from "@material-ui/core";
import MuiAlert from '@material-ui/lab/Alert';

function Copyright() {
    return (
        <Typography variant="body2" color="textSecondary" align="center">
            {'Copyright © Thomas Mathews '}
            {new Date().getFullYear()}
            {'.'}
        </Typography>
    );
}

const useStyles = makeStyles((theme) => ({
    paper: {
        marginTop: theme.spacing(8),
        display: 'flex',
        flexDirection: 'column',
        alignItems: 'center',
    },
    avatar: {
        margin: theme.spacing(1),
        backgroundColor: theme.palette.secondary.main,
    },
    form: {
        width: '100%', // Fix IE 11 issue.
        marginTop: theme.spacing(1),
    },
    submit: {
        margin: theme.spacing(3, 0, 2),
    },
}));

export default function SignIn() {
    const [username, usernameToggle] = React.useState("admin");
    const [password, passwordToggle] = React.useState("admin");
    const [snackbarOpen, snackbarOpenToggle] = React.useState(false);
    const router = useRouter();

    const handleLogin = () => {
        axios.post(`http://localhost:1234/login?username=${username}&password=${password}`, {})
            .then((r) => {
                if (r.data === "Accepted") {
                    return router.push("sites");
                }

                snackbarOpenToggle(true);
            });
    };

    const classes = useStyles();

    return (
        <Container component="main" maxWidth="xs">
            <CssBaseline/>
            <div className={classes.paper}>
                <Avatar className={classes.avatar}>
                    <LockOutlinedIcon/>
                </Avatar>
                <Typography component="h1" variant="h5">
                    Sign in
                </Typography>
                <TextField
                    variant="outlined"
                    margin="normal"
                    required
                    fullWidth
                    id="username"
                    label="Username"
                    name="username"
                    autoFocus
                    value={username}
                    onChange={(e) => usernameToggle(e.target.value)}
                />
                <TextField
                    variant="outlined"
                    margin="normal"
                    required
                    fullWidth
                    name="password"
                    label="Password"
                    type="password"
                    id="password"
                    value={password}
                    onChange={(e) => passwordToggle(e.target.value)}
                />
                <Button
                    type="button"
                    fullWidth
                    variant="contained"
                    color="primary"
                    className={classes.submit}
                    onClick={() => handleLogin()}
                >
                    Sign In
                </Button>
                <Snackbar open={snackbarOpen} autoHideDuration={1000}>
                    <MuiAlert elevation={6} variant="filled" severity="error">
                        Login failed. Check username and password were correct
                    </MuiAlert>
                </Snackbar>
            </div>
            <Box mt={8}>
                <Copyright/>
            </Box>
        </Container>
    );
}

//TODO: Replace icon.