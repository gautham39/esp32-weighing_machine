<!DOCTYPE html>
<html>
<head>
    <title>ESP32 Smart Config and Weight Monitoring with HX711 and MQTT</title>
</head>
<body>

<h1>ESP32 Smart Config and Weight Monitoring with HX711 and MQTT</h1>

<p>This project involves an ESP32 microcontroller that connects to Wi-Fi using Smart Config, reads weight data from a load cell using an HX711 driver, and sends notifications via MQTT when the weight changes by more than 50 grams.</p>

<h2>Table of Contents</h2>
<ul>
    <li><a href="#overview">Overview</a></li>
    <li><a href="#features">Features</a></li>
    <li><a href="#hardware-requirements">Hardware Requirements</a></li>
    <li><a href="#software-requirements">Software Requirements</a></li>
    <li><a href="#setup-instructions">Setup Instructions</a></li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#code-structure">Code Structure</a></li>
    <li><a href="#references">References</a></li>
    <li><a href="#license">License</a></li>
</ul>

<h2 id="overview">Overview</h2>
<p>This project aims to create a smart weighing system using the ESP32 microcontroller. The system connects to Wi-Fi using Smart Config, measures weight using an HX711 load cell amplifier, and notifies the user via MQTT if there is a weight change of more than 50 grams.</p>

<h2 id="features">Features</h2>
<ul>
    <li>Connects to Wi-Fi using Smart Config and MQTT broker</li>
    <li>Measures weight using HX711 and a load cell</li>
    <li>Sends MQTT notifications when weight changes by more than 50 grams</li>
</ul>

<h2 id="hardware-requirements">Hardware Requirements</h2>
<ul>
    <li>ESP32 development board</li>
    <li>HX711 load cell amplifier</li>
    <li>Load cell (e.g., 1kg, 5kg, etc.)</li>
    <li>Wi-Fi network</li>
    <li>MQTT broker</li>
</ul>

<h2 id="software-requirements">Software Requirements</h2>
<ul>
    <li>ESP-IDF (Espressif IoT Development Framework)</li>
    <li>MQTT broker (e.g., Mosquitto)</li>
    <li>Python (for MQTT broker setup and testing)</li>
</ul>

<h2 id="setup-instructions">Setup Instructions</h2>

<h3>Hardware Setup</h3>
<ol>
    <li>Connect the HX711 module to the ESP32:
        <ul>
            <li><code>VCC</code> to <code>3.3V</code></li>
            <li><code>GND</code> to <code>GND</code></li>
            <li><code>DT</code> to <code>GPIO21</code></li>
            <li><code>SCK</code> to <code>GPIO22</code></li>
        </ul>
    </li>
    <li>Connect the load cell to the HX711 module as per the load cell and HX711 documentation.</li>
</ol>

<h3>Software Setup</h3>
<ol>
    <li>Install <a href="https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html">ESP-IDF</a>.</li>
    <li>Clone this repository:
        <pre><code>git clone https://github.com/yourusername/esp32-hx711-mqtt.git
cd esp32-hx711-mqtt
</code></pre>
    </li>
    <li>Set up the ESP-IDF environment:
        <pre><code>. $HOME/esp/esp-idf/export.sh
</code></pre>
    </li>
    <li>Configure the project:
        <pre><code>idf.py menuconfig
</code></pre>
        <ul>
            <li>Set your Wi-Fi credentials under <code>Example Configuration</code>.</li>
            <li>Set your MQTT broker details under <code>MQTT Configuration</code>.</li>
        </ul>
    </li>
    <li>Build and flash the project:
        <pre><code>idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
</code></pre>
    </li>
</ol>

<h2 id="usage">Usage</h2>
<ol>
    <li>Power on the ESP32. It will automatically enter Smart Config mode to connect to the Wi-Fi network.</li>
    <li>Use a Smart Config app (e.g., ESPTouch) to send the Wi-Fi credentials to the ESP32.</li>
    <li>Once connected to Wi-Fi, the ESP32 will be connected to the specified MQTT broker and start measuring weight using the HX711 module.</li>
    <li>If the weight changes by more than 50 grams, an MQTT message will be published to the configured topic (here it is /esp/led_status).</li>
    <li>If we wnat to connect to a new wifi_network a JSON command {smartconfig:start}can be sent to the configured topic to reset the wifi_credentials  (here it is /esp/led_control).</li>
    <li>OTA update is also enabled where if we send  a JSON command {update:"ota_url"} is send the esp 32 is updated  (here it is /esp/led_control).</li>

</ol>

<h2 id="code-structure">Code Structure</h2>
<ul>
    <li><code>main/main.c</code>: Main application code</li>
    <li><code>main/smartconfig.c</code>: Smart Config handling code</li>
    <li><code>main/hx711.c</code>: HX711 driver code</li>
    <li><code>main/mqtt.c</code>: MQTT client code</li>
    <li><code>main/Kconfig.projbuild</code>: Project-specific configuration menu</li>
</ul>

<h2 id="references">References</h2>
<ul>
    <li><a href="https://docs.espressif.com/projects/esp-idf/en/latest/esp32/index.html">ESP-IDF Documentation</a></li>
    <li><a href="https://www.mouser.com/datasheet/2/813/hx711_english-1022875.pdf">HX711 Datasheet</a></li>
    <li><a href="https://mqtt.org/documentation">MQTT Documentation</a></li>
</ul>

<h2 id="license">License</h2>
<p>This project is licensed under the MIT License. See the <a href="LICENSE">LICENSE</a> file for details.</p>

<p>Feel free to open an issue or submit a pull request if you find any bugs or have suggestions for improvements.</p>

<p>Happy coding!</p>

</body>
</html>
