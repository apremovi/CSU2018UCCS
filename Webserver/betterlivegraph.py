import dash #use stuff from dash
from dash.dependencies import Output, Event #to update live, we don't need update, just an event that triggers some sort of function
import dash_core_components as dcc
import dash_html_components as html
import plotly
import random #random data to graph
import plotly.graph_objs as go
from collections import deque #deque is a container where you can specifiy max size. It will pop out the 0th element and input the new element when it hits the max length

X = deque(maxlen=300)
Y = deque(maxlen=300)
X.append(1)
Y.append(1)

app = dash.Dash(__name__)

#styling the html layout
app.layout = html.Div(
    [   html.H2('Your Home Energy Usage'), 
        dcc.Graph(id='live-graph', animate=True), #need an ID to update live, this graph won't show anything until the callback funciton calls figure
        
        dcc.Interval(
            id='graph-update',#this is how we're going to trigger an event to occur
            interval = 3000 #update every 30 second or 3000 ms this graph is going to run with the id = update graph
            )
        ]
    )

@app.callback(Output('live-graph', 'figure'),
                  events = [Event('graph-update', 'interval')])
def update_graph(): #wrapper being called, encasing the function within
        global X
        global Y
        X.append(X[-1]+1)
        Y.append(Y[-1]+(Y[-1]*random.uniform(-0.1,0.1)))

        data = go.Scatter(
            x = list(X),
            y = list(Y),
            name = 'Energy Usage',
            mode = 'lines+markers' #this is the same as a line graph
            
            )

        return {'data':[data], 'layout': go.Layout(xaxis = dict(range=[min(X), max(X)],title = 'Time'),
                                                   yaxis = dict(range=[min(Y), max(Y)],title = 'Energy Consumption(kW)'))}
if __name__ == "__main__":
    app.run_server(debug=True)
