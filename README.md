# ESP32 Smart Config and Weight Monitoring with HX711 and MQTT

This project involves an ESP32 microcontroller that connects to Wi-Fi using Smart Config, reads weight data from a load cell using an HX711 driver, and sends notifications via MQTT when the weight changes by more than 50 grams.


Hardware Requirements- 
ESP32 development board
HX711 load cell amplifier
Load cell (e.g., 1kg, 5kg, etc,here 40kg load cell is used)
Wi-Fi network
MQTT broker


Usage
1.Power on the ESP32. It will automatically enter Smart Config mode to connect to the Wi-Fi network.
2.Use a Smart Config app (e.g., ESPTouch) to send the Wi-Fi credentials to the ESP32.
3.Once connected to Wi-Fi, the ESP32 will be connected to the specified MQTT broker and start measuring weight using the HX711 module.
4.If the weight changes by more than 50 grams, an MQTT message will be published to the configured topic.
5. {smartconfig : start} json command can be sent using mqtt broker on esp/led_control topic to reset smartconfig credentials and connect to a new network.


Code Structure
1.main/main.c: Main application code
2.main/smartconfig.c: Smart Config handling code
3.main/hx711.c: HX711 driver code
4.main/mqtt.c: MQTT client code
5.main/Kconfig.projbuild: Project-specific configuration menu
