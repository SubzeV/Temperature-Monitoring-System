// Make sure to include these libraries in your Platform.io project before compiling!
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

// WiFi settings
const char* ssid = "wifi"; // Change this to your WiFi network
const char* password = "password"; // Change this to your WiFi password

// MQTT settings
const char* mqtt_server = "mqttserver.local"; // Change this to your mqtt server adress (examples: "192.168.0.xx" or "mqtt.local")
const int mqtt_port = 1883; // If you use a different MQTT port, change that here
const char* mqtt_username = "username"; // Change this to your MQTT username
const char* mqtt_password = "password"; // Change this to your MQTT password
const char* mqtt_topic = "pc/temp/intake"; // Change this if you wish to output the temperature to another place (useful if you want multiple sensors in different areas, example: "pc/temp/exhaust")

// Initialize the BMP280 sensor
Adafruit_BMP280 bmp;

// Initialize the MQTT client
WiFiClient espClient;
PubSubClient client(espClient);

unsigned long publishMillis = 0;

const long publishInterval = 2000; // Interval to publish average temperature (milliseconds) change this if you require more or less data per time interval

unsigned long sensorReadMillis = 0;
const long sensorReadInterval = 100; // Interval to read sensor (milliseconds) change this to a higher number if you want the sensor to be read less times a second (might lower accuracy but will reduce calculation load on the D1)

float temperatureSum = 0;
int readingsCount = 0;

// Set up the WiFi connection
void setup_wifi() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { // wait for connection
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected"); // Debug prints for WiFi connection
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// Reconnect if connection lost
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client", mqtt_username, mqtt_password)) {
      Serial.println("connected"); // Debug print for successful MQTT connection
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds"); // Debug print for failed MQTT connection
      delay(5000);
    }
  }
}

// Start the wifi and MQTT connection
void setup() {
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  while (!bmp.begin(0x76)) {
    Serial.println("Could not find a valid BMP280 sensor, check wiring!"); // Debug print for failing to find BMP280, will retry after 2 seconds
    delay (2000);
  }
}

// Read the sensors and publish the data
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long currentMillis = millis();

  // Read sensor 20 times per second
  if (currentMillis - sensorReadMillis >= sensorReadInterval) {
    sensorReadMillis = currentMillis;
    float temperature = bmp.readTemperature();
    temperatureSum += temperature;
    readingsCount++;
  }

  // Publish average temperature every 2 seconds
  if (currentMillis - publishMillis >= publishInterval) {
    publishMillis = currentMillis;
    if (readingsCount > 0) {
      float averageTemperature = temperatureSum / readingsCount;
      client.publish(mqtt_topic, String(averageTemperature).c_str());
      temperatureSum = 0;
      readingsCount = 0;
    }
  }
}