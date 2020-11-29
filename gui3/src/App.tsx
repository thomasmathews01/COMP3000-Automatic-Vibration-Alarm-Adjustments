import * as React from "react"
import LoginPage from "./Pages/LoginPage";
import SitesPage from "./Pages/SitesPage";
import {BrowserRouter, Route, Switch} from "react-router-dom";
import SitePage from "./Pages/SitePage"
import ChannelPage from "./Pages/ChannelPage";
import MachinePage from "./Pages/MachinePage";

export const App: React.FC = () => (
    <BrowserRouter>
        <Switch>
            <Route path="/channel/:id" children={<ChannelPage/>}/>
            <Route path="/machine/:id" children={<MachinePage/>}/>
            <Route path="/site/:id" children={<SitePage/>}/>
            <Route path="/sites" children={<SitesPage/>}/>
            <Route path="/" children={<LoginPage/>}/>
        </Switch>
    </BrowserRouter>
)
