#include <Arduino.h>
#include <DHT_U.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define DHTTYPE    DHT22     // DHT 22 (AM2302)
#define DHTPIN D1

// WiFi settings
const char* ssid = "";
const char* password = "";
const char* mqtt_server = "";
const char* mqtt_user = "";
const char* mqtt_pass = "";

// Time to sleep (60 seconds):
const int sleepTimeS = 60e6;
char temp[8];
char humidity[8];
char voltage[6];

DHT_Unified dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {

  delay(10);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(50);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(2000);

  // Wait for serial to initialize.
  while(!Serial) { }

  setup_wifi();
  client.setServer(mqtt_server, 1883);

  dht.begin();
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  dtostrf(event.temperature, 4, 2, temp);
  dht.humidity().getEvent(&event);
  dtostrf(event.relative_humidity, 4, 2, humidity);

  int nVoltageRaw = analogRead(A0);
  // float fVoltage = (float)nVoltageRaw * 0.00686;
  float fVoltage = (float)nVoltageRaw * 0.00486;
  dtostrf(fVoltage, 4, 2, voltage);

  randomSeed(micros());
  String clientId = "ESP8266Meteo-";
  clientId += String(random(0xffff), HEX);
  if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
    client.publish("stat/meteo/temperature", temp);
    client.publish("stat/meteo/humidity", humidity);
    client.publish("stat/meteo/voltage", voltage);
  }
  client.disconnect();

  ESP.deepSleep(sleepTimeS); 
  
  // Deep sleep mode until RESET pin is connected to a LOW signal (for example pushbutton or magnetic reed switch)
  //Serial.println("I'm awake, but I'm going into deep sleep mode until RESET pin is connected to a LOW signal");
  //ESP.deepSleep(0); 
}

void loop() {
  // put your main code here, to run repeatedly:
}