import * as React from "react"
import LoginPage from "./Pages/LoginPage";
import SitesPage from "./Pages/SitesPage";
import {BrowserRouter, Route, Switch} from "react-router-dom";

export const App: React.FC = () => (
    <BrowserRouter>
        <Switch>
            <Route path="/sites">
                <SitesPage/>
            </Route>
            <Route path="/">
                <LoginPage/>
            </Route>
        </Switch>
    </BrowserRouter>
)
