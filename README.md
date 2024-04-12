# Project info
## What do I need for this project?
- An arduino uno R4 WiFi
- One or more ESP32s (Devboard V1, etc.)
- One or more ESP8266s (D1 Mini, etc.)
- I2C LCD (20x4)
- I2C Oled display (128x64 px)
- 4 Buttons (Preferably in red, yellow, green and blue to more easily distinguish the options in the menus)
- BMP280 sensor(s)
- Wiring (Breadboard(s), jumper wires, etc.)
- A Raspberry Pi to use as an MQTT server and Grafana host
- A WiFi connection that allows devices to talk to each other over the local network (Most home WiFi networks should allow this by default)

### Documentation will be available under the `documentation/` directory.

## How to use?
Download all the files from the source (or releases tab for stable releases) and upload each piece of code to the correct microcontroller.

Each file will also specify WiFi and MQTT details, please ensure you enter the correct details in there before uploading your code to the microcontroller.

All files will be in the `.cpp` format as I have written these in Visual Studio Code using Platform.io, however using Arduino IDE is also possible if you have the correct drivers installed. Each file of code also comes with a `ReadMe.md` and an image guide to connect the components as specified in the code.

Please note, this guide assumes you already have a bit of experience working with either Arduino IDE or VSCode Platform.io and the hardware.

## How to control the settings for the display:
Each menu option will be displayed with either a 'R>', 'Y>' or 'G>'. These correspond to Red, Yellow and Green as I have wired up these buttons for selections. (Refer to the build instructions if you want the colors to match.)
The blue button will be your back button

You can also add buttons in your Blynk app that change virtual values `V1`, `V2` and `V3` from 0 to 1 when pressed to change the displayed temperatures.


