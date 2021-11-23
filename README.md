# Vindriktning-plus
Updated Vindriktning with Wifi Connectivity, Motion sensor, Temperature and Humidity

Inspired & parts of the code are used from: https://github.com/Hypfer/esp8266-vindriktning-particle-sensor

This is my "Backpack" for the Vindriktning PM25 sensor. Housing is adjusted to the IKEA housing.
A standalone version for nodeMcu boards is available with addtional LUX sensor. (V1: analog sensor)

Current setup sends temperature, humidity and pm25 values every 30 seconds to HomeAssistant MQTT server.
Every 1 second PIR motion sensor is checked, in case motion is detected an mqtt message is send.

Work is still in progress.

Addtional to the print you need:
- Wemos D1
- AHT21 sensor
- Mini PIR sensor
- 4 x M2x10 self tapping

Check, update and rename secret_template.h to secret.h!


![View_Front](./img/View_Front.jpg)

![View_Back](./img/View_Back.jpg)

Who needs color coding! Used some old leftover wire...

![Electronics](./img/Electronics.jpg)

Connect Vindriktning and Backpack via VHB tape, any other glue will work too.

![VHB-Tape](./img/VHB-Tape.jpg)

Before closing the packpack, close the connection hole with hotglue to prevent warm air circulation through the backpack.

![Hot_glue_hole](./img/Hot_glue_hole.jpg)