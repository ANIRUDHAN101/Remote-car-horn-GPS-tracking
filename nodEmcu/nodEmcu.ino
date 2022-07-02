#include <Arduino.h>
#include <TinyGPS++.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>
 
TinyGPSPlus gps;
/*d7 - 13-tx
  d8 - 15-rx*/
SoftwareSerial gpsSerial(15, 13); // RX, TX
SoftwareSerial ardSerial(4,5);
char buff[100];
const char* ssid = "Galaxy M30s11E5";

const char* password = "ynwo4606";
const char* mqtt_server = "public.mqtthq.com";
//test.mosquitto.org
//broker.hivemq.com
//mqtt.flespi.io
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void callback(char* topic, byte* payload, unsigned int length)
{
    payload[length] = '\0';
    float value = String((char*) payload).toFloat();
 
    Serial.println(topic);
    Serial.println(value);
    ardSerial.print("/");
    ardSerial.print(value);
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("Car1\\near");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void printData() 
{
    if (gps.location.isUpdated()) 
    {
        double lat = gps.location.lat();
        double lng = gps.location.lng();
        //double lat = 76.110909;
        //double lng = 10.110090;
         
        snprintf(buff,  MSG_BUFFER_SIZE,"%f",lat);
        Serial.print("Publish message: ");
        Serial.print(buff);
        client.publish("Car1\\latitude", buff);
        snprintf (buff, MSG_BUFFER_SIZE, "%f", lng);
         Serial.print(" ");
       
        Serial.println(buff);
        client.publish("Car1\\longitude", buff);
    }
}
 
void setup() 
{
    Serial.begin(9600);
    setup_wifi();
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
    gpsSerial.begin(9600);
    ardSerial.begin(9600);
    
}
 
void loop() 
{
   //Serial.println(rec);
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
    while (gpsSerial.available() > 0) {
        if (gps.encode(gpsSerial.read())) {
            printData();
        }
    }
}
