from __future__ import print_function
from apiclient.discovery import build
from httplib2 import Http
from oauth2client import file, client, tools
import sys
import serial
import atexit
import time
from datetime import date, datetime
import sys, time, msvcrt

def readInput( caption, default, timeout = 5):
    start_time = time.time()
    sys.stdout.write('%s(%s):'%(caption, default))
    sys.stdout.flush()
    input = ''
    while True:
        if msvcrt.kbhit():
            byte_arr = msvcrt.getche()
            if ord(byte_arr) == 13: # enter_key
                break
            elif ord(byte_arr) >= 32: #space_char
                input += "".join(map(chr,byte_arr))
        if len(input) == 0 and (time.time() - start_time) > timeout:            
            break    
    if len(input) > 0:
        return input
    else:
        return default



cansat_names = ['CanSatCR','SulaBatsu','Cansat2','Cansat4','Cansat6','Cansat5']

ser = serial.Serial('COM4',115200,timeout=1)  # open serial port
print(ser.name)         # check which port was really used
#ser.write(b'hello')     # write a string
@atexit.register
def goodbye():
    ser.close()             # close port when exiting the program


# Setup the Sheets API
SPREADSHEET_ID = '13KYNKFreWm4SWH6Ra5fFqb8qi5KZ7VOrwdeU28r4jnM'
SCOPES = 'https://www.googleapis.com/auth/spreadsheets'
store = file.Storage('credentials.json')
creds = store.get()
if not creds or creds.invalid:
    flow = client.flow_from_clientsecrets('client_secret.json', SCOPES)
    creds = tools.run_flow(flow, store)
service_sheets = build('sheets', 'v4', http=creds.authorize(Http()))



# Definition of main functions
def getLastPosition(cansat_index):    
    # Call the Sheets API
    RANGE_NAME = str(cansat_index)+'!A2:B'
    result = service_sheets.spreadsheets().values().get(spreadsheetId=SPREADSHEET_ID,
                                                 range=RANGE_NAME).execute()
    values = result.get('values', [])
    i = 1
    if not values:
        print('No data found.')
    else:       
        for row in values:        
            i=i+1
    return i;

def uploadTelemetry(cansat_index):    
    position = getLastPosition(cansat_index)    
    values = [
        [
            str(datetime.today()),num_beacon,temp,hum,tih,tif,latitude,longitude,altitude,cam_status,rssi_cansat,rssi_gs,       ]        
    ]
    body = {
        'values': values
    }
    range_name = str(cansat_index)+'!A'
    range_name = range_name + str(position)
    value_input_option = 'USER_ENTERED'
    spreadsheet_id = '13KYNKFreWm4SWH6Ra5fFqb8qi5KZ7VOrwdeU28r4jnM'
    result = service_sheets.spreadsheets().values().append(
        spreadsheetId=spreadsheet_id, range=range_name,
        valueInputOption=value_input_option, body=body).execute()
    print('>Telemetry Uploaded in sheet:' + str(cansat_index) +'\n') 
    return;

while True:    
    buffer_in = str(ser.read(150))
    telemetry=str(buffer_in[buffer_in.find("Received")+9:])
    #print(buffer_in+'\n')
    if len(telemetry)>2:
        print('\n'+telemetry)    
        cansat_name= telemetry[:telemetry.find("#")]
        data = telemetry[telemetry.find("#")+1:].split(',')
        if len(data)>2:
            num_beacon = data[0]
            temp = data[1]
            hum = data[2]
            tih = data[3]
            tif = data[4]
            latitude = data[5]
            longitude = data[6]            
            altitude = data[7]       
            cam_status = data[8]                 
            rssi_cansat = data[9]
            if data[10][0] != "-":
                rssi_gs = data[10].split('\\')[2][data[10].split('\\')[2].find("RSSI:")+5:]
            else:
                rssi_gs = "X"      
            uploadTelemetry(cansat_names.index(cansat_name))     
        #print(data)
        # Collect events until released

    
    cmd_input = readInput('\nCMD_Mission:', 5 ) 
    print(cmd_input)
    if (cmd_input=='c'):
        ser.write(cmd_input.encode())
        print('Turning ON video...')
    elif (cmd_input=='x'):        
        ser.write(cmd_input.encode())
        print('Turning OFF video...')
    

                                  
  