/*
WiFi 101 ThingSpeak Data Uploader
Hardware Required:
* Arduino Zero or Uno Board
* Arduino Wifi Sheild 101
* Photocell
* Temperature Sensor (This example uses a TMP36)
* 10K Ohm Resistor

created Sept 2015
by Helena Bisby <support@arduino.cc>
This example code is in the public domain
http://arduino.cc/en/Tutorial/WiFi101ThingSpeakDataUploader

*/

// ESP8266-dht11-thingspeak
// Sending temperature and humidity data to ThingSpeak using ESP8266 and DHT-11 module.

#include <ESP8266WiFi.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

#define DHTPIN 2     // Digital pin connected to the DHT sensor

// Uncomment the type of sensor in use:
#define DHTTYPE    DHT11     // DHT 11

// deep sleep time in microseconds:
const unsigned int DEEP_SLEEP_TIME = 900e6;

// Local Network Settings

// Replace with your network credentials
const char* ssid = "ssid";
const char* password = "password";

int status = WL_IDLE_STATUS;

DHT dht(DHTPIN, DHTTYPE);

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;    // will store last time DHT was updated

// Updates DHT readings every 10 seconds
const long interval = 10000; 

// current temperature & humidity, updated in loop()
float t = 0.0;
float h = 0.0;

WiFiServer server(80);

// ThingSpeak Settings
char thingSpeakAddress[] = "api.thingspeak.com";
String APIKey = "API_KEY";                      // enter your channel's Write API Key
const int updateThingSpeakInterval = 20 * 1000; // 20 second interval at which to update ThingSpeak

// Variable Setup
long lastConnectionTime = 0;
boolean lastConnected = false;

// Initialize Arduino Ethernet Client
WiFiClient client;

void setup() {
  // Start Serial for debugging on the Serial Monitor
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  dht.begin();

   // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(".");
  }

  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());
  // you're connected now, so print out the status:
  //printWifiStatus();
}

void loop() {
  // read values from pins and store as strings
  
  processDHTresults();
  ESP.deepSleep(DEEP_SLEEP_TIME);
  
}

void processDHTresults() {
    // Read temperature as Celsius (the default)
    float newT = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    //float newT = dht.readTemperature(true);
    // if temperature read failed, don't change t value
    if (isnan(newT)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      t = newT;
      Serial.println(t);
    }
    // Read Humidity
    float newH = dht.readHumidity();
    // if humidity read failed, don't change h value 
    if (isnan(newH)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      h = newH;
      Serial.println(h);
    }
    updateThingSpeak("field1=" + String(t) + "&field2=" + String(h));  
}


void updateThingSpeak(String tsData) {
  if (client.connect(thingSpeakAddress, 80)) {
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + APIKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(tsData.length());
    client.print("\n\n");
    client.print(tsData);
    lastConnectionTime = millis();

    if (client.connected()) {
      Serial.println("Connecting to ThingSpeak...");
      Serial.println();
    }
  }
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
