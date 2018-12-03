import requests
import bs4 
import time
import pandas as pd
from datetime import datetime
import glob
import os




def hasNumbers(inputString):
    return any(char.isdigit() for char in inputString)

def get_count():
    url = "http://10.0.0.206/temp_report.html"

    # request with fake header, otherwise you will get an 403 HTTP error
    r = requests.get(url, headers={'User-Agent': 'Mozilla/5.0'})
    page_source = r.text
    # print(page_source)

    soup = bs4.BeautifulSoup(page_source, 'html.parser')
    #print(soup)


    data = soup.find_all('p')

    # Pull time
    data_time = data[0].text.split()
    data_time = [element for element in data_time if hasNumbers(element)][0]

    # Pull date
    data_date = data[1].text.split()
    data_date = [element for element in data_date if hasNumbers(element)][0]

    # Convert date and time to datetime data type
    data_datetime = data_date +' '+ data_time
    data_datetime = datetime.strptime(data_datetime, '%Y-%m-%d %H:%M:%S')

    # Pull usage and convert to data type int
    data_usage = data[2].text.split()
    data_usage = [element for element in data_usage if hasNumbers(element)][0]
    data_usage = int(data_usage)


    temp_df = pd.DataFrame(data = [[data_time, data_usage]], columns = ['time', 'kW'] )
    print('Date: %s     Time: %s      Usage: %s kW' %(data_date, data_time, data_usage))
    return temp_df



# change path_to_file string
path_to_file = 'C:/seniord/csusite/'	

# If folder doesn't exist, create it
if not os.path.exists(path_to_file):
    os.makedirs(path_to_file)

# get all files in that folder
fnames = glob.glob('energy_usage.csv')

# if there are no files, start with new dataframe
# else, get the most recently saved data and read that to continue appending
if fnames == []:
    results = pd.DataFrame()
else: 
    latest_file = max(fnames, key=os.path.getctime)
    results = pd.read_csv(path_to_file+latest_file)


date_stored = datetime.now().strftime('%Y_%m_%d_%HH_%M_%S')	
while True:
    df = get_count()
    results = results.append(df).reset_index(drop=True)
    #results.to_csv(path_to_file+'%s_kW_usage.csv' %(date_stored),index=False) #If you want to create a new csv file to the folder, uncomment this. Otherwise use the code below
    results.to_csv(path_to_file+'energy_usage.csv',index=False)
    time.sleep(4)
