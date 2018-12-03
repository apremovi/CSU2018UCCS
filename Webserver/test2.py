import dash #use stuff from dash; Dash helps with data visualization and embedding that in some sort of browser-based UI(You dont have to be connected to the web). Dash incorporates HTML with REACT and javascript and the plotly graph. 
import dash_core_components as dcc
import dash_html_components as html
from dash.dependencies import Input, Output #to update live, we don't need update, just an event that triggers some sort of function

import pandas as pd
import plotly.graph_objs as go
import datetime




app = dash.Dash(__name__)
#styling the html layout within python
#REMEMBER: All the updating that's going to be done via Javascript.
app.layout = html.Div(
    html.Div([
        html.H4('Your Home Energy Usage'),
        dcc.Graph(id='live-update-graph'), #need an ID to update live, this graph won't show anything until the callback funciton calls figure. The id here is live-graph

        dcc.Interval(
            id='interval-component', #this is how we're going to trigger an event to occur. An event that has the id of graph-update, but it's an interval
            interval= 2000, # in milliseconds #update every 30 second or 3000 ms this graph is going to run with the id = update graph
            n_intervals=0
        )
    ])
)

@app.callback(Output('live-update-graph', 'figure'), #Make a callback here and say that the Output is this live graph(we want to update whatever has the id of live-graph), and 
              [Input('interval-component', 'n_intervals')]) #event of the ID graph-update
def update_graph_live(n): #defines the actual function intself. #The wrapper itself is being called, so it's almost as if it's encasing the function within, so it doesn't matter what we call this function

    df = pd.read_csv(
    'C:\\seniord\\csusite\\energy_usage.csv')

    

    # Create the graph with subplots
    #Return this dictionary that's called data (above)
    #When you animate a graph with plotly, it doesn't refresh the entire graph, just the data points in the graph. So to update the x and y axis too, we do 'go.Layout(xaxis= dict(range=[min(X)]))]'
    fig =   {
                'data': [
                        {'x': df.time,'y': df.kW, 'type': 'line', 'name': 'Hourly Average'},
                        ],
                'layout': go.Layout (
                            title= datetime.datetime.now().strftime('DATE: %m-%d-%Y'),
                xaxis={'title': 'Time(UTC)'},
                yaxis={'title': 'Energy Consumption(kW)'},
                
            )
                            
                          }
    

    return fig



if __name__ == '__main__':
    app.run_server(debug=True)
