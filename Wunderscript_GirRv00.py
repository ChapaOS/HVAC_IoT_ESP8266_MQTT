
# Anthony Chapa 11/17/2020
# Wunderscript is a program to stream data into WU_Update Object
# Rv00 - 11/17/20 - Initial GitHub Release
# 

import paho.mqtt.client as mqtt
from WU_Update import WU_Update

MQTT_ADDRESS = '192.168.1.200'
MQTT_USER = 'IoTbot'
MQTT_PASSWORD = 'berry123'
MQTT_TOPIC = 'home/+/+'              #all topics
#MQTT_TOPIC = 'home/node003/ATOTx76' #specific sensor

sen_id = "ATOTx76"                  #specified weather Sensor
sen_elev = 237.5                    #Sensor elevation in m above sea level
WU_ID = "Your Info"                 #Personal WU Station ID
WU_PW = "Your Info"                 #WU Station Password

WU = WU_Update(sen_id,sen_elev,WU_ID,WU_PW)
 
    
def on_connect(client, userdata, flags, rc):
    """ The callback for when the client receives a CONNACK response from the server."""
    print('Connected with result code ' + str(rc))
    client.subscribe(MQTT_TOPIC)

def on_message(client, userdata, msg):
    """The callback for when a PUBLISH message is received from the server."""
    #print(msg.topic + str(msg.payload))
    payload_string=msg.payload.decode('utf-8')
    #print("payload: " + payload_string)
    WU.update(payload_string)

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
        main()
    except:
        pass
