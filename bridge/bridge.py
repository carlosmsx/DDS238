import os
import time
import sys
import paho.mqtt.client as mqtt
import json
import serial

port = serial.Serial('/dev/ttyACM0', 115200)


THINGSBOARD_HOST = '127.0.0.1'
ACCESS_TOKEN = 'fvfyNYEiHz28IgPxGx6X'

# Data capture and upload interval in seconds. Less interval will eventually hang the DHT22.
INTERVAL=2

sensor_data = {'temperature': 0, 'humidity': 0}

next_reading = time.time() 

client = mqtt.Client()

# Set access token
client.username_pw_set(ACCESS_TOKEN)

# Connect to ThingsBoard using default MQTT port and 60 seconds keepalive interval
client.connect(THINGSBOARD_HOST, 1883, 60)

client.loop_start()

try:
    while True:
        try:
            binstr = port.readline()
            #remover ultimo caracter
            binstr = binstr[:-1]
            #convierto a string
            cadena = binstr.decode('utf-8')
            #convierto a diccionario
            meter_readings = eval(cadena)
            
            for k,v in meter_readings.items():
            #    publish.single("myhouse/energy/%s"%k, "%s"%v, hostname="127.0.0.1")
                print("%s: %s" % (k,v))
        
            # Sending humidity and temperature data to ThingsBoard
            client.publish('v1/devices/me/telemetry', json.dumps(meter_readings), 1)

            next_reading += INTERVAL
            sleep_time = next_reading-time.time()
            if sleep_time > 0:
                time.sleep(sleep_time)
        except:
            print("error")
except KeyboardInterrupt:
    pass

client.loop_stop()
client.disconnect()
meter.close