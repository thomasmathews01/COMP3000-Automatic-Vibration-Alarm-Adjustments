import * as React from "react"
import {createBrowserHistory} from 'history'
import LoginPage from "./Pages/LoginPage";
import SitesPage from "./Pages/SitesPage";

const history = createBrowserHistory()

interface RouterState {
    url: string
}

const Router: React.FC<{}> = props => {
    const [state, setState] = React.useState<RouterState>({url: window.location.pathname})

    React.useEffect(() => history.listen(e => setState(s => s.url !== e.pathname ? {url: e.pathname} : s)))

    if (Array.isArray(props.children)) {
        const children = props.children.map<React.ReactNode>((n: React.ReactNode) => {
            let c = n as React.ReactElement;
            if (c === undefined || c === null || c.props === undefined) return c;

            if (c.type === Route && c.props.path !== state.url) return <></>
            return c
        })
        return <>{children}</>
    }
    return <>{props.children}</>
}

interface LinkProps {
    to: string
}

const Link: React.FC<LinkProps> = props => (
    <a
        href={props.to}
        onClick={e => {
            e.preventDefault()
            history.push(props.to)
        }}
    >{props.children}</a>
)

export const App: React.FC<{}> = () => (
    <main>
        <Link to={'/'}>Home</Link>
        <Link to={'/sites'}>Sites</Link>
        <Router>
            <Route path="/"><LoginPage/></Route>
            <Route path="/sites"><SitesPage/></Route>
        </Router>
    </main>
)

interface RouteProps {
    path?: string
}

class Route extends React.Component<RouteProps> {
    render() {
        return this.props.children
    }
}