// Make sure to include these libraries in your Platform.io project before compiling!
#include <SPI.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <U8g2lib.h>

// Define display object with the I2C address 0x3C
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE, /* clock=*/SCL, /* data=*/SDA);

#define WIFI_SSID "wifiname" // Change this to your WiFi network
#define WIFI_PASSWORD "password" // Change this to your WiFi password

#define MQTT_BROKER "mqttserver.local" // Change this to your mqtt server adress (examples: "192.168.0.xx" or "mqtt.local")
#define MQTT_PORT 1883 // If you use a different MQTT port, change that here
#define MQTT_CLIENT_ID "arduino_client" // This has to be unique so if you have multiple of these, please change it for each one individually

#define MQTT_USERNAME "username" // Change this to your MQTT username
#define MQTT_PASSWORD "password" // Change this to your MQTT password

// Define the topics
#define MQTT_SCREEN_GPU_TOPIC "pc/screen/gpu" // This is the topic to receive what should be displayed on the screen
#define MQTT_INTAKE_TEMP_TOPIC "pc/temp/intake" // This is the topic that sends through intake temperatures
#define MQTT_EXHAUST_TEMP_TOPIC "pc/temp/exhaust" // This is the topic that sends through exhaust temperatures

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

int displayMode = 2; // Variable to store the current mode

float temperatureIntake = 0.0;
float temperatureExhaust = 0.0;

void connectWiFi();
void callback(char *topic, byte *payload, unsigned int length);
void display();

void setup()
{
  Serial.begin(9600);
  connectWiFi();
  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  mqttClient.setCallback(callback);

  while (!mqttClient.connected())
  {
    if (mqttClient.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD))
    {
      Serial.println("Connected to MQTT broker");
      mqttClient.subscribe(MQTT_SCREEN_GPU_TOPIC);
      mqttClient.subscribe(MQTT_INTAKE_TEMP_TOPIC);
      mqttClient.subscribe(MQTT_EXHAUST_TEMP_TOPIC);
    }
    else
    {
      Serial.println("Failed to connect to MQTT broker. Retrying...");
      delay(5000);
    }
  }

  // Initialize the I2C communication
  Wire.begin();
  // Initialize the display
  u8g2.begin();
}

void loop()
{
  display();

  mqttClient.loop();
}

void connectWiFi()
{
  Serial.print("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
}

void callback(char *topic, byte *payload, unsigned int length)
{
  String message = "";
  for (unsigned int i = 0; i < length; i++)
  {
    message += (char)payload[i];
  }

  Serial.print("Received message on topic: ");
  Serial.print(topic);
  Serial.print(", message: ");
  Serial.println(message);

  // Check the received message and update the linked variable
  if (strcmp(topic, MQTT_INTAKE_TEMP_TOPIC) == 0)
  {
    // Convert the message to float and update the temperatureIntake variable
    temperatureIntake = atof(message.c_str());
    Serial.print("Temperature: ");
    Serial.println(temperatureIntake);
  }
  else if (strcmp(topic, MQTT_EXHAUST_TEMP_TOPIC) == 0)
  {
    // Convert the message to float and update the temperatureExhaust variable
    temperatureExhaust = atof(message.c_str());
    Serial.print("Temperature: ");
    Serial.println(temperatureExhaust);
  }
  else if (strcmp(topic, MQTT_SCREEN_GPU_TOPIC) == 0)
  {
    if (message == "intake")
    {
      displayMode = 0;
    }
    else if (message == "exhaust")
    {
      displayMode = 1;
    }
    else if (message == "both")
    {
      displayMode = 2;
    }
  }

  Serial.print("Exhaust mode: ");
  Serial.println(displayMode);
}

void display()
{
  u8g2.clearBuffer();

  // Set font and position for text
  u8g2.setFont(u8g2_font_ncenB08_tr);

  // Display temps
  if (displayMode == 0)
  {
    u8g2.setCursor(0, 10);
    u8g2.print("Intake:");

    u8g2.setCursor(0, 20);
    u8g2.print(temperatureIntake, 1);
    u8g2.print(" °C");
  }
  else if (displayMode == 1)
  {
    u8g2.setCursor(0, 10);
    u8g2.print("Exhaust:");

    u8g2.setCursor(0, 20);
    u8g2.print(temperatureExhaust, 1);
    u8g2.print(" °C");
  }
  else if (displayMode == 2)
  {
    u8g2.setCursor(15, 10);
    u8g2.print("Exhaust");

    u8g2.setCursor(58, 10);
    u8g2.print("|");

    u8g2.setCursor(74, 10);
    u8g2.print("Intake");

    

    u8g2.setCursor(16, 20);
    u8g2.print(temperatureExhaust, 1);
    u8g2.print(" °C");

    u8g2.setCursor(58, 20);
    u8g2.print("|");

    u8g2.setCursor(74, 20);
    u8g2.print(temperatureIntake, 1);
    u8g2.print(" °C");
  }

  // Send the text to the screen
  u8g2.sendBuffer();
}
