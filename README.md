# HVAC_IoT_ESP8266_MQTT
Revision: 0
Date: 11/16/2020
Contributor: Chapa

The goal of this project is to set up a Raspberry Pi to Collect, Log and Analyze BME280 sensor data streams from ESP8266 units installed upstream and downstream of an Air Conditioning Unit.

At this time 11/16/2020, I currently have scripts for Objectives 1 & 3 complete.
I am currently revising objective 2 to have an optional self-updating matplotlib graphs for troubleshooting while Objective 4 is currently in development. I want to release clean working models of 1-3 prior to posting to GitHub and creating the helping Wiki Page.

Objectives:
1. Create a script that can log the BME280 sensor data points to a MQTT server and save to a CSV file.
2. Create a script that can detect and monitor AC events documenting Avg T,P,H as well as the duration of each AC event.
3. Create a script that can filter out atmospheric data and post this as a Personal Weather Station onto Wunderground.com. 
4. Create a script that can detect Air Filter cleanliness and alert if change is needed.
5. Develop all scripted objects to be able to handle live data streams as well as previously recorded data for analysis.

Challenges:
1. Atmospheric barometric pressure can shift 1000 pa/day when the AC status change signal is 30 pa and pressure noise can be around 10-20 pa. Filtering out local noise and environmental shift is a challenge
2. AC humidity in subterranean vents often hits 100% humidity, thus sensors may see liquid water if improperly placed.
3. Sensor power logistics and setup can be a challenge.
4. Cheap 4$ boards and 3$ sensors can sometimes be fickle little creatures.

Method:
1. We will set up NodeMCU ESP8266 units with Bosch BME280 sensors taking measurements every 15s.
2. When taking a measurement, the ESP8266 will ping the local NTP server for a timestamp of the measurement.
3. We will combine these measurements with a signal strength indicator and timestamp into a data string and post this message onto a MQTT server running on a Raspberry Pi.
4. The Pi will listen on the MQTT server and will log any sensor post.
5. The Pi will also run a script to detail AC run events and log metrics.

Files Available.
1. MCU_Send_Data_GitRv00 - NodeMCU ESP8266 Arduino script file for sensor data gathering and posting.
2. DataLogger_GitRv00.py - Python Script to Scan MQTT server and Log Live Sensor Data
3. DataViewer_GitRv00.py - Python Script to Scan MQTT server Only View Live Data Stream
4. Wunderscript_GitRv00.py - Python Script to Scan MQTT server and post to Wunderground.com
5. WU_Update.py - Python class to calculate and post weather data
6. stormcalc.py - Python weather helper functions

Setup Instructions to be revised shortly.
1. Set up Raspberry Pi to install mosquitto server and NTP Server
2. Ensure the servers are running.
3. Install/load code onto NodeMCU devices. Edit wifi info as necessary. Test sensors prior to installation.
4. Log into Wunderground.com and create a Personal Weather station. Should be able to obtain key and password.
5. Update and Run Python scripts with Key and Password above and updated wifi network info.


Notes:
I would recommend utilizing an isolated router for this project. 
* It isolates the ESP8266 units away from the interwebs.
* Utilizing a local NTP server on this network reduces the lag time for timestamping.
* If the internet goes down, the data server/logging is completely unaffected.

References:
Thank you to all of the authors that made the following posts.
I am forever grateful.
If you recognize any snippets of code not referenced in this list, please let me know.

First : Adafruit.com
Without this website, my love for circuits, sensors, learning and blue smoke would not be the same.
The BME driver was used from their Arduino package.

Initial Start Up Blink
https://lastminuteengineers.com/esp8266-nodemcu-arduino-tutorial/

BME280 Set Up
https://lastminuteengineers.com/bme280-esp8266-weather-station/

NTP Ideas
https://lastminuteengineers.com/esp8266-ntp-server-date-time-tutorial/

NTP Code Snipet
https://randomnerdtutorials.com/esp8266-nodemcu-date-time-ntp-client-server-arduino/

MQTT Info
https://diyi0t.com/introduction-into-mqtt/

MQTT/Mosquitto Setup/Python MQTT Code
https://diyi0t.com/microcontroller-to-raspberry-pi-wifi-mqtt-communication/

Raspberry Pi Time Info
https://raspberrytips.com/time-sync-raspberry-pi/




