#define BLYNK_TEMPLATE_ID   "user7"
#define BLYNK_TEMPLATE_NAME "user7@server.wyns.it"
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <PubSubClient.h>

char auth[] = "auth_code"; // Replace with your own auth code found in the Blynk app
char ssid[] = "wifi"; // Put your WiFi name here
char pass[] = "wifi_pass"; // Put your WiFi password here

const char* mqtt_server = "mqttserver.local"; // Change this to your MQTT server
const int mqtt_port = 1883;
const char* mqtt_username = "mqtt_user"; // Change to your MQTT user
const char* mqtt_password = "mqtt_password"; // Change to your MQTT password
const char* mqtt_topic_intake = "pc/temp/intake";
const char* mqtt_topic_exhaust = "pc/temp/exhaust";
const char* mqtt_topic_room = "room/temp/general";
const char* mqtt_topic_screen_gpu = "pc/screen/gpu";

WiFiClient espClient;
PubSubClient client(espClient);

Adafruit_BMP280 bmp; // Initialize the BMP280 sensor

unsigned long publishMillis = 0;
const long publishInterval = 250;

float temperatureIntake;
float temperatureExhaust;
float temperatureRoom;

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client1", mqtt_username, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 3 seconds");
      delay(3000);
    }
  }
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Received message on topic: ");
  Serial.print(topic);
  Serial.print(", message length: ");
  Serial.print(length);
  Serial.print(", message: ");
  
  // Add null terminator to payload to treat it as a string
  payload[length] = '\0';
  
  // Print payload as a string
  Serial.println((char*)payload);
}

void setup() {
  Serial.begin(9600);
  delay(10);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  int wifi_ctr = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Blynk.begin(auth, ssid, pass, "server.wyns.it", 8081);
  Serial.println("Blynk connected");
  client.setServer(mqtt_server, mqtt_port); 
  Serial.println("MQTT connected");
  client.setCallback(callback);
  client.subscribe(mqtt_topic_intake);
  client.subscribe(mqtt_topic_exhaust);
  client.subscribe(mqtt_topic_room);

  // Initialize the BMP280 sensor
  while (!bmp.begin(0x76)) {
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    delay(2000);
  }
}

void loop() {
  Blynk.run();
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long currentMillis = millis();

  // Read temperature data from BMP280 sensor
  float temperature = bmp.readTemperature();
  temperatureExhaust = temperature; // Update exhaust temperature

  // Publish temperature to Blynk V0
  Blynk.virtualWrite(V0, temperatureExhaust);

  // Publish temperature data to MQTT topics
  if (currentMillis - publishMillis >= publishInterval) {
    publishMillis = currentMillis;
    client.publish(mqtt_topic_exhaust, String(temperatureExhaust).c_str());
  }
}

BLYNK_WRITE(V1) {
  int value = param.asInt();
  Serial.print("V1 value is: ");
  Serial.println(value);
  if (value == 1) {
    client.publish(mqtt_topic_screen_gpu, "intake");
  }
}

BLYNK_WRITE(V2) {
  int value = param.asInt();
  Serial.print("V2 value is: ");
  Serial.println(value);
  if (value == 1) {
    client.publish(mqtt_topic_screen_gpu, "exhaust");
  }
}

BLYNK_WRITE(V3) {
  int value = param.asInt();
  Serial.print("V3 value is: ");
  Serial.println(value);
  if (value == 1) {
    client.publish(mqtt_topic_screen_gpu, "both");
  }
}
