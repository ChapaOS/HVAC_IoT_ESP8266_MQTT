/*
Anthony Chapa 11/17/2020
This Program is to distribue to all the nodes sending Sensor Info
11/17/20 - Initial GitHub Release

*/

#include <Adafruit_Sensor.h> // Adafurit Sensor Interface Package
#include <Adafruit_BME280.h> // Adafruit BME280 Driver Code
#include <ESP8266WiFi.h>     // Enables the ESP8266 to connect to the local network (via WiFi)
#include <PubSubClient.h>    // Connects and publish to the MQTT broker
#include <NTPClient.h>       // NTP Client functions
#include <WiFiUdp.h>         // Wifi UDP functions

/* 
Sensor Network Map Notes
   Static IP  =  Topic  =  SubTopics 1/2  = Delay = Description
192.168.1.199 = node000 =     PubTest     =    0s =       Test Module/No Sensors
192.168.1.201 = node001 = AC1xx76/AC1xx77 =    3s =   Downtown AC - AC1xx76=in, AC1xx77=out
192.168.1.202 = node002 = AC2xx76/AC2xx77 =    7s =     Uptown AC - AC2xx76=in, AC2xx77=out
192.168.1.203 = node003 = ATOTx76/ATOTx77 =    9s = Attic/Outside - ATOTx76=outside ATOTx77=attic east
192.168.1.204 = node004 = ATTTx76/ATTTx77 =   11s = Attic/AtticHi - ATOTx76=attic west ATOTx77=attic high

*/

//Individual Node Module Setup Items

IPAddress staticIP(192,168,1,204);  // This Module Static IP on Wifi

String nodeName = "node001";        //MCU Module Name Node Topic Handle
String sen1Name = "AC1xx76";        //MCU Sensor Name Sensor 1 SubTopic Handle
String sen2Name = "AC1xx77";        //MCU Sensor Name Sensor 2 SubTopic Handle

unsigned long delayTime =3;         // Sets program start time shift from Powerup
unsigned long cycleTime =12;        // Sets data update cycle time, 17s = 20s w/ sensors



// WiFi Setup Constants
const char* ssid = "Your_Network_SSID";        // Your personal network SSID
const char* wifi_password = "pass_code";       // Your personal network password

IPAddress gateway(192,168,1,1);                // Your Router DNS Gateway 
IPAddress subnet(255,255,255,0);               // Your Router Submask
IPAddress mqtt_server(192,168,1,200);          // IP of the MQTT broker & NTP server (Raspberry Pi)
const char* ntp_server = "192.168.1.200";      // Easier to type once as String rather than function

// MQTT/NTP Constants
const char* mqtt_username = "IoTbot";          // MQTT username
const char* mqtt_password = "berry123";        // MQTT password
const char* clientID = "client_node";          // MQTT client ID

// MQTT Setup Strings
String topic1 = "home/"+nodeName+"/"+sen1Name; // Sensor 1 Topic String to Publish On
String topic2 = "home/"+nodeName+"/"+sen2Name; // Sensor 2 Topic String to Publish On
const char* sensor001_topic = topic1.c_str();  // Topic 1 convert to Char
const char* sensor002_topic = topic2.c_str();  // Topic 2 convert to Char

//Placeholder State Vars
unsigned status1;         // Wifi Connection Status 
long rssi = 0;            // Wifi Signal Strength Status for Datastream
String liveIP = "";       // Wifi IP Status for Datastream

//I2C Object Setup
Adafruit_BME280 bme1;     // I2C Object for Sensor 1
Adafruit_BME280 bme2;     // I2C Object for Sensor 2

//Client Service Setup
WiFiClient wifiClient;                                   // Initialize the WiFi and MQTT Client objects
PubSubClient client(mqtt_server, 1883, wifiClient);      // 1883 is the listener port for the Broker
   
WiFiUDP ntpUDP;                                          // Initialize the WiFiUDP Object
NTPClient timeClient(ntpUDP, ntp_server, 0);             // Initialize NTP Server Client (using pi ip)


// Custom function to connet to the MQTT broker via WiFi
void connect_MQTT(){
  Serial.print("Connecting to ");
  
  // Connect to the WiFi
  WiFi.mode(WIFI_STA);                             // Sets WiFi Connection Type
  WiFi.setPhyMode(WIFI_PHY_MODE_11B);              // Sets TX to 802.11 b spec 11B/11G/11N
  WiFi.setOutputPower(20.5);                       // Sets TX Power 0-20.5 dBm @ .25 Steps
  WiFi.begin(ssid, wifi_password);                 // Start Wifi Connection
  WiFi.config(staticIP,gateway,subnet);            // Set Static IP
  Serial.println(ssid);
 
  // Wait until the connection has been confirmed before continuing & flash while Connecting
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, LOW);               //Antenna LED HIGH/LOW = OFF/ON
    delay(10);
    digitalWrite(LED_BUILTIN, HIGH);              //Antenna LED HIGH/LOW = OFF/ON
    delay(90);
    Serial.print(".");
  }

  // Debugging - Output the IP Address and RSSI of the ESP8266
  liveIP = WiFi.localIP().toString().c_str();
  rssi = WiFi.RSSI();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(liveIP);
  Serial.print("IP RSSI: ");
  Serial.println(rssi);

  // Connect to NTP Server
  timeClient.begin();

  // Connect to MQTT Broker
  // client.connect returns a boolean value to let us know if the connection was successful.
  // If the connection is failing, make sure you are using the correct MQTT Username and Password 
  
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker Failed...");
  }
}


void setup() {
    Serial.begin(9600);
    while(!Serial);                       // Wait for serial to start running
    Serial.println(F("BME280 Setup"));    // Debug Note
    
    unsigned stat1;                       // Sensor 1 Status State Variable
    unsigned stat2;                       // Sensor 2 Status State Variable

    stat1 = bme1.begin(0x76);             // stat1 bool error if sensor 1 error is found
    
    if (!stat1) {
        Serial.println("Could not find a valid BME280 sensor 1, check wiring, address, sensor ID!");
        Serial.print("SensorID was: 0x"); Serial.println(bme1.sensorID(),16);
        Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
        Serial.print("        ID of 0x60 represents a BME 280.\n");
        Serial.print("        ID of 0x61 represents a BME 680.\n");
        while (1) delay(10);
    }

    stat2 = bme2.begin(0x77);             // stat2 bool error if sensor 2 error is found
    
    if (!stat2) {
        Serial.println("Could not find a valid BME280 sensor 2, check wiring, address, sensor ID!");
        Serial.print("SensorID was: 0x"); Serial.println(bme2.sensorID(),16);
        Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
        Serial.print("        ID of 0x60 represents a BME 280.\n");
        Serial.print("        ID of 0x61 represents a BME 680.\n");
        while (1) delay(10);
    }

    Serial.println(" ");
    Serial.println("Starting MCU Sensor Setup");
    
    Serial.println("------ Sensor 1 Setup ------");
    Serial.print("Reporting on Topic 1: ");
    Serial.println(sensor001_topic);
    Serial.println("forced mode, 16x pressure / 16x temperature / 16x humidity oversampling,");
    Serial.println("20ms standby period, filter off");
    bme1.setSampling(Adafruit_BME280::MODE_FORCED,  // MODE_NORMAL or MODE_FORCED
                     Adafruit_BME280::SAMPLING_X16,  // Temp SAMPLING_NONE, SAMPLING_X1,X2,X4,X8,X16
                     Adafruit_BME280::SAMPLING_X16,  // Pres SAMPLING_NONE, SAMPLING_X1,X2,X4,X8,X16
                     Adafruit_BME280::SAMPLING_X16,  // Humi SAMPLING_NONE, SAMPLING_X1,X2,X4,X8,X16
                     Adafruit_BME280::FILTER_OFF,   // FILTER_OFF or FILTER_X2,X4,X8,X16
                     Adafruit_BME280::STANDBY_MS_20 ); // 20ms Std.By.

    Serial.println("------ Sensor 2 Setup ------");
    Serial.print("Reporting on Topic 2: ");
    Serial.println(sensor002_topic);
    Serial.println("forced mode, 16x pressure / 16x temperature / 16x humidity oversampling,");
    Serial.println("20ms standby period, filter off");
    bme2.setSampling(Adafruit_BME280::MODE_FORCED,  // MODE_NORMAL or MODE_FORCED
                     Adafruit_BME280::SAMPLING_X16,  // Temp SAMPLING_NONE, SAMPLING_X1,X2,X4,X8,X16
                     Adafruit_BME280::SAMPLING_X16,  // Pres SAMPLING_NONE, SAMPLING_X1,X2,X4,X8,X16
                     Adafruit_BME280::SAMPLING_X16,  // Humi SAMPLING_NONE, SAMPLING_X1,X2,X4,X8,X16
                     Adafruit_BME280::FILTER_OFF,   // FILTER_OFF or FILTER_X2,X4,X8,X16
                     Adafruit_BME280::STANDBY_MS_20 ); // 20ms Std.By.

    // suggested max polling rate is 25Hz
    // 1 + (2 * T_ovs) + (2 * P_ovs + 0.5) + (2 * H_ovs + 0.5)
    // T_ovs = 4
    // P_ovs = 4
    // H_ovs = 4
    // = 26ms (25Hz)
    
    Serial.println();
    delay(delayTime*1000);           // delayTime is the initial startup delay
    pinMode(D0, OUTPUT);             // Assigns LED pin D0 as Output
    pinMode(LED_BUILTIN, OUTPUT);    // Assigns LED_Builtin pin as Output
    digitalWrite(D0, HIGH);          // Turns off D0 USB LED (HIGH/LOW = OFF/ON) 
    digitalWrite(LED_BUILTIN, HIGH); // Turns off Antenna LED (HIGH/LOW = OFF/ON)
}

void loop() {

  connect_MQTT();                                        // Runs the MQTT Connect function
  Serial.setTimeout(500);

  timeClient.update();                                   // Updates the NTP TimeClient Object Time
  unsigned long epochTime = timeClient.getEpochTime();   // Stores the EpochTime as Long Number
  String datTime = String((long)epochTime);              // Converts Number to String

  bme1.takeForcedMeasurement();                          // Needed in Forced Mode, has no effect in Normal Mode
  bme2.takeForcedMeasurement();                          // Needed in Forced Mode, has no effect in Normal Mode

  float t1 = bme1.readTemperature();           // Pulls Temp Reading from Sensor 1 (*C)
  float t2 = bme2.readTemperature();           // Pulls Temp Reading from Sensor 2 (*C)

  float p1 = (bme1.readPressure() / 100.0F);   // Pulls Press Reading from Sensor 1 (hPa)
  float p2 = (bme2.readPressure() / 100.0F);   // Pulls Press Reading from Sensor 2 (hPa)

  float h1 = bme1.readHumidity();              // Pulls Humid Reading from Sensor 1 (%rh)
  float h2 = bme2.readHumidity();              // Pulls Humid Reading from Sensor 2 (%rh)


  // MQTT can only transmit strings
  String sen001Data = sen1Name+","+liveIP+","+String((long)rssi)+","+datTime+","+String((float)t1)+","+String((float)p1)+","+String((float)h1);
  String sen002Data = sen2Name+","+liveIP+","+String((long)rssi)+","+datTime+","+String((float)t2)+","+String((float)p2)+","+String((float)h2);
  
  Serial.println(sen001Data);
  Serial.println(sen002Data);
  
  // PUBLISH to the MQTT Broker (topic = Sensor1, defined at the beginning)
  // client.publish returns a boolean on weather it succedes or not.
  // If Publish Fails, It will attempt to Reconnect and Try Again Once as Connection may have broke.
  
  if (client.publish(sensor001_topic, String(sen001Data).c_str())) {
    Serial.println(sen1Name+" data sent!");
  }
  else {
    Serial.println("sen001Data failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(5000); // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(sensor001_topic, String(sen001Data).c_str());
  }
  
  // PUBLISH to the MQTT Broker (topic = Sensor2)
  
  if (client.publish(sensor002_topic, String(sen002Data).c_str())) {
    Serial.println(sen2Name+" data sent!");
  }
  else {
    Serial.println("sen002Data failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(5000); // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(sensor002_topic, String(sen002Data).c_str());
  }

  client.disconnect();     // disconnect from MQTT broker
  digitalWrite(D0, LOW);   // Flash D0 LED to show cycle complete
  delay(50);
  digitalWrite(D0, HIGH);  // Turn off D0 Light
  delay(cycleTime*1000);   //cycleTime sets Loop Speed 17s = 20s cycle time
}
