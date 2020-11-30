import * as React from "react"
import {BrowserRouter, Route, Switch} from "react-router-dom";
import {OuterPage} from "./Components/OuterPage";
import {MachinePage} from "./Pages/MachinePage";
import {SitePage} from "./Pages/SitePage";
import {SitesPage} from "./Pages/SitesPage";
import {ChannelPage} from "./Pages/ChannelPage";
import {LoginPage} from "./Pages/LoginPage";

export const App: React.FC = () => (
    <BrowserRouter>
        <Switch>
            <Route path="/channel/:id" children={<OuterPage><ChannelPage/></OuterPage>}/>
            <Route path="/machine/:id" children={<OuterPage><MachinePage/></OuterPage>}/>
            <Route path="/site/:id" children={<OuterPage><SitePage/></OuterPage>}/>
            <Route path="/sites" children={<OuterPage><SitesPage/></OuterPage>}/>
            <Route path="/" children={<LoginPage/>}/>
        </Switch>
    </BrowserRouter>
)
