// Make sure to include these libraries in your Platform.io project before compiling!
#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>

#define WIFI_SSID "wifiname" // Change this to your WiFi network
#define WIFI_PASSWORD "password" // Change this to your WiFi password

#define MQTT_BROKER "mqttserver.local" // Change this to your mqtt server adress (examples: "192.168.0.xx" or "mqtt.local")
#define MQTT_PORT 1883 // If you use a different MQTT port, change that here
#define MQTT_CLIENT_ID "esp32_control_client" // This has to be unique so if you have multiple of these, please change it for each one individually
#define MQTT_SCREEN_GPU_TOPIC "pc/screen/gpu" // This is the topic to send what should be displayed on the screen
#define MQTT_TEMP_TOPIC "pc/temps/exhaust" // This is the topic that recieves intake temperatures
#define MQTT_GPU_TOPIC "pc/screen/gpu" // This is the topic that recieves exhaust temperatures
#define MQTT_USERNAME "username" // Change this to your MQTT username
#define MQTT_PASSWORD "password" // Change this to your MQTT password

// Set the LCD address (most commonly 0x27)
#define LCD_ADDRESS 0x27

// Set the LCD dimensions (number of columns and rows) - This project requires at least a 20x4 display!
#define LCD_COLUMNS 20
#define LCD_ROWS 4

// Initialize the LCD object
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Menu options lists, these can be modified to add more or change exisiting options in each menu
const char *mainMenuList[] = { // Main menu options
    "Settings",
    "Live monitor",
    "Extras"};

const char *settingsMenuList[] = { // Settings menu options
    "Oled settings",
    "Sensor settings",
    "Other settings"};

const char *oledSettingsMenuList[] = { // Oled settings menu options
    "Display Exhaust",
    "display Intake",
    "Display Both"};

const char *readingsMenuList[] = { // Readings menu options
    "Temperatures",
    "Humidity",
    "Other"};

const int mainMenuListSize = sizeof(mainMenuList) / sizeof(mainMenuList[0]); // define list size

// const int settingsMenuListSize = sizeof(settingsMenuList) / sizeof(settingsMenuList[0]); // define list size

const int buttonA = 15;
const int buttonB = 4;
const int buttonX = 5;
const int buttonY = 18;

int stateButtonA = LOW;
int stateButtonB = LOW;
int stateButtonX = LOW;
int stateButtonY = LOW;

int lastStateButtonA = HIGH;
int lastStateButtonB = HIGH;
int lastStateButtonX = HIGH;
int lastStateButtonY = HIGH;

int menu = 0; // Store what menu is currently being displayed
bool changeMenu = false;

bool selectedAnOption = false;

void initializeText();
void menuPrint(int menuChoice);
void mainMenuPrint();
void settingsMenuPrint();
void oledSettingsMenuPrint();
void readingsMenuPrint();
void clearScreen();
void buttonChecks();

void connectWiFi();
void reconnect();
void callback(char *topic, byte *payload, unsigned int length);

void setup()
{
  Serial.begin(9600);

  pinMode(buttonA, INPUT_PULLUP);
  pinMode(buttonB, INPUT_PULLUP);
  pinMode(buttonX, INPUT_PULLUP);
  pinMode(buttonY, INPUT_PULLUP);

  // Initialize the LCD
  lcd.init();

  // Turn on the backlight
  lcd.backlight();

  // Display 'Initializing' text
  initializeText();

  // Initialize WiFi & MQTT
  connectWiFi();
  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  mqttClient.setCallback(callback);

  // Print Main menu
  menuPrint(0);
}

void loop()
{
  if (!mqttClient.connected())
  {
    reconnect();
  }

  mqttClient.loop();

  buttonChecks();

  if (changeMenu == true)
  {
    menuPrint(menu);
    changeMenu = false;
  }
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
  Serial.println("Connected!");
}

void reconnect()
{
  while (!mqttClient.connected())
  {
    Serial.print("Attempting MQTT connection...");
    if (mqttClient.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD))
    {
      Serial.println("connected");
      mqttClient.subscribe(MQTT_GPU_TOPIC);
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char *topic, byte *payload, unsigned int length)
{
  String message;
  for (unsigned int i = 0; i < length; i++)
  {
    message += (char)payload[i];
  }

  if (String(topic) == MQTT_GPU_TOPIC)
  {
    if (message == "test")
    {
      // do something
    }
    else
    {
      // Handle other message types as needed
    }
  }
}

void initializeText()
{
  clearScreen();
  lcd.setCursor(0, 1); // Set cursor to the first column and second row
  lcd.print("-=| Initializing |=-");
  lcd.setCursor(0, 2); // Set cursor to the first column and third row
  lcd.print("-=| Connecting.. |=-");
}

void menuPrint(int menuChoice)
{
  if (menuChoice == 0) // Main menu
  {
    mainMenuPrint();
  }
  else if (menuChoice == 1) // Settings menu
  {
    settingsMenuPrint();
  }
  else if (menuChoice == 2) // Oled Settings menu
  {
    oledSettingsMenuPrint();
  }
  else if (menuChoice == 3) // Live Readings menu
  {
    readingsMenuPrint();
  }
  else if (menuChoice == 4) // Live Readings menu
  {
    readingsMenuPrint();
  }

  for (int i = 0; i < 3; i++)
  {
    lcd.setCursor(0, i + 1); // Set cursor to first row
    if (i == 0 && menuChoice != 4)
    {
      lcd.print("R> "); // Print the button to select
    }
    else if (i == 1 && menuChoice != 4)
    {
      lcd.print("Y> "); // Print the button to select
    }
    else if (i == 2 && menuChoice != 4)
    {
      lcd.print("G> "); // Print the button to select
    }
    if (menuChoice == 0) // Main menu
    {
      lcd.print(mainMenuList[i]); // Print the Main menu options
    }
    else if (menuChoice == 1) // Settings menu
    {
      lcd.print(settingsMenuList[i]); // Print the Settings menu options
    }
    else if (menuChoice == 2) // Oled Settings menu
    {
      lcd.print(oledSettingsMenuList[i]); // Print the Oled Settings menu options
    }
    else if (menuChoice == 3) // Readings menu
    {
      lcd.print(readingsMenuList[i]); // Print the Readings menu options
    }
    else if (menuChoice == 4) // Readings Temps Settings menu
    {
      lcd.setCursor(0, 1);
      lcd.print("   Room: "); // Print the Temps
      lcd.print("22.1 C   ");

      lcd.setCursor(0, 2);
      lcd.print(" Intake: ");
      lcd.print("22.5 C   ");

      lcd.setCursor(0, 3);
      lcd.print("Exhaust: ");
      lcd.print("35.8 C   ");
    }
  }
  
}

void mainMenuPrint()
{
  clearScreen();
  lcd.setCursor(0, 0); // Set cursor to the first column and first row
  lcd.print("--=| Main  menu |=--");
}

void settingsMenuPrint()
{
  clearScreen();
  lcd.setCursor(0, 0); // Set cursor to the first column and first row
  lcd.print("---=| Settings |=---");
}

void oledSettingsMenuPrint()
{
  clearScreen();
  lcd.setCursor(0, 0); // Set cursor to the first column and first row
  lcd.print("-----=| OLED |=-----");
}

void readingsMenuPrint()
{
  clearScreen();
  delay(10);
  lcd.setCursor(0, 0); // Set cursor to the first column and first row
  lcd.print("---=| Readings |=---");
}

void clearScreen()
{
  lcd.clear();
}

void buttonChecks()
{
  stateButtonA = digitalRead(buttonA);
  stateButtonB = digitalRead(buttonB);
  stateButtonX = digitalRead(buttonX);
  stateButtonY = digitalRead(buttonY);

  // ----- Check for button A -----

  if (stateButtonA == LOW && lastStateButtonA == HIGH)
  {
    changeMenu = true;
    if (menu == 2) // On Oled Settings menu
    {
      mqttClient.publish(MQTT_SCREEN_GPU_TOPIC, "both"); // Change Oled screen
    }
    else if (menu == 0)
    {
      // extra option selected, currently not implemented
    }
    else if (menu == 1)
    {
      menu = 1; // other settings menu, currently not implemented
    }
    lastStateButtonA = LOW;
    delay(25); // Debounce delay
  }
  if (stateButtonA == HIGH && lastStateButtonA == LOW)
  {
    lastStateButtonA = HIGH;
    delay(25); // Debounce delay
  }

  // ----- Check for button B -----

  if (stateButtonB == LOW && lastStateButtonB == HIGH)
  {
    changeMenu = true;
    if (menu == 3) // On Readings menu
    {
      menu = 4;
    }
    else if (menu == 2) // On Oled Settings menu
    {
      mqttClient.publish(MQTT_SCREEN_GPU_TOPIC, "exhaust"); // Change Oled screen
    }
    else if (menu == 1) // On Settings menu
    {
      menu = 2;
    }
    else if (menu == 0) // On Main menu
    {
      menu = 1;
    }
    lastStateButtonB = LOW;
    delay(25); // Debounce delay
  }
  if (stateButtonB == HIGH && lastStateButtonB == LOW)
  {
    lastStateButtonB = HIGH;
    delay(25); // Debounce delay
  }

  // ----- Check for button X -----

  if (stateButtonX == LOW && lastStateButtonX == HIGH)
  {
    changeMenu = true;
    menu = 0; // Go to main menu
    lastStateButtonX = LOW;
    delay(25); // Debounce delay
  }
  if (stateButtonX == HIGH && lastStateButtonX == LOW)
  {
    lastStateButtonX = HIGH;
    delay(25); // Debounce delay
  }

  // ----- Check for button Y -----

  if (stateButtonY == LOW && lastStateButtonY == HIGH)
  {
    if (menu == 2) // On Oled Settings menu
    {
      mqttClient.publish(MQTT_SCREEN_GPU_TOPIC, "intake"); // Change Oled screen
    }
    else if (menu == 0) // On Main menu
    {
      menu = 3;
    }
    changeMenu = true;
    lastStateButtonY = LOW;
    delay(25); // Debounce delay
  }
  else if (stateButtonY == HIGH && lastStateButtonY == LOW)
  {
    lastStateButtonY = HIGH;
    delay(25); // Debounce delay
  }
}
