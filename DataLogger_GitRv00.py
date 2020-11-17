
# Anthony Chapa 11/17/2020
# This script is to display data from MQTT Stream and Log it for Data Collection
# Rv00 - 11/17/20 - Initial GitHub Release
# 


import paho.mqtt.client as mqtt
import os.path
from datetime import date

start_date = date.today()
date_str = start_date.strftime("%m%d%y")

LOG_FILE_PATH = "/home/pi/Projects/Data_Archive/"
LOG_FILE_NAME = "Sensor_Log"
LOG_FILE_DATE = "_"+date_str

LOG_FILE_FULLPATH = LOG_FILE_PATH + LOG_FILE_NAME + LOG_FILE_DATE + ".csv"

MQTT_ADDRESS = '192.168.1.200'
MQTT_USER = 'IoTbot'
MQTT_PASSWORD = 'berry123'
MQTT_TOPIC = 'home/+/+'

def file_check():
    print("\n-- Confirming Logging File Location --")
    if(not os.path.exists(LOG_FILE_FULLPATH)):
        f=open(LOG_FILE_FULLPATH,"a+")
        f.write("SensorName,Local_IP,RSSI,EpochTime,Temp_C,Pres_hPa,Humid_%\n")
        f.close()
        print("File created @ Path: " + LOG_FILE_FULLPATH + "\n")
    else:
        print("File present @ Path: " + LOG_FILE_FULLPATH + "\n")

def on_connect(client, userdata, flags, rc):
    """ The callback for when the client receives a CONNACK response from the server."""
    print('Connected with result code ' + str(rc))
    client.subscribe(MQTT_TOPIC)


def on_message(client, userdata, msg):
    """The callback for when a PUBLISH message is received from the server."""
    payload_str=msg.payload.decode('utf-8')
    print(msg.topic + ' ' + payload_str)
    f=open(LOG_FILE_FULLPATH,"a+")
    f.write(payload_str+"\n")
    f.close()

def main():
    mqtt_client = mqtt.Client()
    mqtt_client.username_pw_set(MQTT_USER, MQTT_PASSWORD)
    mqtt_client.on_connect = on_connect
    mqtt_client.on_message = on_message

    mqtt_client.connect(MQTT_ADDRESS, 1883)
    mqtt_client.loop_forever()


if __name__ == '__main__':
    try:
        print('MQTT to InfluxDB bridge')
        file_check()
        main()
    except ValueError:
        f.close()
