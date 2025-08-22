# logger script to execute the notebook and log output to a file with name as the timestamp

# Libraries
# import numpy as np
# import pandas as pd
# import matplotlib.pyplot as plt
import time
import serial

# Configs:
# Serial port
port = '/dev/ttyUSB0'
filename = time.strftime('%Y-%m-%d_%H:%M:%S') + '.csv'

# Serial port config
baudrate = 115200
timeout = 1
ser = serial.Serial(port, baudrate, timeout=timeout)

# initialize all variables
uid = 0
tempNTC = 0
windspeed = 0
temp = 0
hum = 0
pres = 0
gas = 0

# Helper functions

# Read and return the entire serial buffer
def read_serial():
    ser.flushInput()
    ser.flushOutput()
    # wait for data
    while ser.inWaiting() == 0:
        pass
    time.sleep(0.2)
    # return the entire buffer
    return ser.read_all()

# Determine whether it is wind or temp by inspecting the ?uid tag
# return 0 for invalid data, 1 for temp, 2 for wind
def parse_data(data):
    data = data.decode('utf-8')
    # only parse the data if it has ? in the beginning
    if '?uid' not in data:
        return 0
    # find the uid tag, begin with ?uid= end with &
    uid = data.split('uid=')[1].split('&')[0]
    if 'uid=temp' in data:
        return 1
    elif 'uid=wind' in data:
        return 2
    else:
        return 0

# Parse the temperature data and return the temperature value
# return uid, tempNTC
def parse_data_temp(data):
    # format: abcxyz...?temp=xx.xxabcxyz -> xx.xx
    # NEW example payload: ?uid=temp0&tempNTC=-272.15&
    data = data.decode('utf-8')
    # split the data with each parameter and ends with & to get the value
    # strip data before parsing
    data = data.split('\r\n')[3]
    # print("stripped data: ", data)

    try:
        uid = data.split('uid=')[1].split('&')[0]
    except:
        uid = -1
    
    try:
        tempNTC = float(data.split('tempNTC=')[1].split('&')[0])
    except:
        tempNTC = -1
    return uid, tempNTC

# Parse the wind data and return the wind speed value
# return uid, windspeed, temperature, humidity, pressure, gas
def parse_data_wind(data):
    # format: abcxyz...?uid=abcxyz...&windspeed=xx.xxabcxyz... -> xx.xx...
    # ?uid=wind1&windspeed=0.00&temperature=23.47&humidity=51.61&pressure=1011.30&gas=67.87&
    data = data.decode('utf-8')
    # strip data before parsing
    data = data.split('\r\n')[3]
    # print("stripped data: ", data)

    try:
        uid = data.split('uid=')[1].split('&')[0]
    except:
        uid = -1
    
    try:
        windspeed = float(data.split('windspeed=')[1].split('&')[0])
    except:
        windspeed = -1

    try:
        temp = float(data.split('temperature=')[1].split('&')[0])
    except:
        temp = -1

    try:
        hum = float(data.split('humidity=')[1].split('&')[0])
    except:
        hum = -1

    try:
        pres = float(data.split('pressure=')[1].split('&')[0])
    except:
        pres = -1

    try:
        gas = float(data.split('gas=')[1].split('&')[0])
    except:
        gas = -1

    return uid, windspeed, temp, hum, pres, gas

# Main loop

# check if serial port exists, else print error and exit
if ser.is_open:
    print("Serial port opened successfully")
else:
    print("Error opening serial port")
    exit()

# Check filename
print("Logging data to file: ", filename)

# Open the file and write the header
f = open(filename, 'w')
f.write('Date,Time,uid,TempNTC,WindSpeed,Temperature,Humidity,Pressure,Gas\n')

while True:
    timestamp_csv = time.strftime('%Y-%m-%d,%H:%M:%S')
    timestamp_print = time.strftime('%Y-%m-%d %H:%M:%S') # time in human readable format
    try:
        # read the serial data
        data = read_serial()
        # print(data)

        # check data type
        data_type = parse_data(data)
        # print(data_type)

        # call the appropriate function to parse the data
        if data_type == 1: # temperature data
            uid, tempNTC = parse_data_temp(data)
            print(timestamp_print, uid, tempNTC)
        elif data_type == 2: # wind data
            uid, windspeed, temp, hum, pres, gas = parse_data_wind(data)
            print(timestamp_print, uid, windspeed, temp, hum, pres, gas)
        else:
            continue # skip the loop if data is invalid

        # check for invalid data
        if uid == -1 or tempNTC == -1 or windspeed == -1 or temp == -1 or hum == -1 or pres == -1 or gas == -1:
            continue # skip the loop if data is invalid

        # write the data to the file
        f.write(f'{timestamp_csv},{uid},{tempNTC},{windspeed},{temp},{hum},{pres},{gas}\n')

    except KeyboardInterrupt:
        f.close()
        ser.close()
        print("Exiting...")
        break
    except Exception as e:
        print(e)
        continue