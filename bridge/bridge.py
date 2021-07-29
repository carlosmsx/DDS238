import serial
#import context  # Ensures paho is in PYTHONPATH
import paho.mqtt.publish as publish

port = serial.Serial('/dev/ttyACM0', 115200)

i=0
while True:
    try:
        binstr = port.readline()
        print(i)
        i=i+1
        #remover ultimo caracter
        binstr = binstr[:-1]
        #convierto a string
        cadena = binstr.decode('utf-8')
        #convierto a diccionario
        meter_readings = eval(cadena)
        
        for k,v in meter_readings.items():
            publish.single("myhouse/energy/%s"%k, "%s"%v, hostname="127.0.0.1")
            print("%s: %s" % (k,v))
    except:
        print("error")

meter.close()