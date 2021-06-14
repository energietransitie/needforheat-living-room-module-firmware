# Twomes CO2-sensor
This repository contains the software for the CO2-measurement devices. The software is based on ESP-IDF and is written for the ESP32.

## Table of contents
* [General info](#general-info)
* [Deploying](#deploying)
* [Developing](#developing) 
* [Features](#features)
* [Status](#status)
* [License](#license)
* [Credits](#credits)

## General info
This software is responsible for managing the CO2-measurement device it is loaded on. It will gather data on the CO2-concentration, temperature and humidty data. 

The data can be sent to the server in two ways. The first option is to send the data to the P1-gateway, which will send the data to the server. Another option would be that the data is sent to the server directly via Wi-Fi. 

The data gathered will be analyzed to see when a room is being ventilated. This makes it possible to give advice about the environmental friendliness of your home.

## Deploying
Currently, there are no binary releases since you need to configure the MAC-addresses for ESP-NOW manually.

### Prerequisites
*   A device based on the ESP32 SoC, like the [LilyGO TTGO T7 Mini32 V1.3 ESP32](https://github.com/LilyGO/ESP32-MINI-32-V1.3) is required;
*   An [SCD41](https://www.sensirion.com/en/environmental-sensors/carbon-dioxide-sensors/carbon-dioxide-sensor-scd4x/) should be connected via I2C to the device;
*   A Micro USB cable to connect the device;
*   A PC with USB port;
*	[Python version 3.8 or above](https://docs.python.org/3/using/windows.html) installed, make sure to add the path to the Python executable to your PATH variable so you can use Python commands from the command prompt;
*	[Esptool](https://github.com/espressif/esptool) installed, the Espressif SoC serial bootloader utility;
*	[Arduino IDE](https://www.arduino.cc/en/software) or [PlatformIO plugin](https://platformio.org/install/ide?install=vscode) for [Visual Studio Code](https://code.visualstudio.com/download) installed.

## Developing

### Prerequisites
* All prerequisites mentioned in [Deploying](#deploying)
*	[Visual Studio Code](https://code.visualstudio.com/download) installed
*	[PlatformIO for Visual Studio Code](https://platformio.org/install/ide?install=vscode) installed
*	A local clone of this GitHub repository

### Building and flashing the project
Open the project in PlatformIO:
  1. In the top-left corner, select File -> Open Folder.
  2. Select the `co2_sensor` folder where you cloned or extracted the repository.
  3. Change any code as you see fit.
  4. Click the PlatformIO Icon (the little alien in the left of the screen).
  5. Unfold `ttgo-t7-v13-mini32`.
  6. Click `upload and monitor`. 
NOTE: The first time might take a while because PlatformIO needs to install and build the ESP-IDF before flashing.

OPTIONAL: When it is done flashing, press `CTRL+T` and then `B`, then type `115200` so that it sets the right baud rate and you see text not gibberish.

### Changes necessary to get ESP-NOW working
In order for ESP-NOW to work on your device you will need to change a few things in `src/espnow.c`. 

Change the `MAC_ADDR_TEST_SENDER` and `MAC_ADDR_TEST_RECVR` to the MAC-address of your chosen sender and receiver, respectively. Make sure to leave the format the same (i.e. an array initialization using curly brackets `{ }`). You can find the MAC-address of your device by flashing this software to both devices. In the serial monitor, you should be able to see a line resembling something like this: `wifi:mode : sta (xx:xx:xx:xx:xx:xx)`. Use the number in between the normal brackets `( )` as the MAC-address for the device type in question (sender/receiver).

Optionally, in `src/espnow.c`, change the `WIFI_CHANNEL` define to a different channel. 

### Toggling the ESP-NOW code between sender and receiver
If you want to use a receiver to test this code, you can uncomment line 16 in `platformio.ini`: `#build_flags = -D ESP_NOW_RECEIVER`. Flash it to your chosen device and it will behave as a receiver.

## Features
*   Measure CO2, temperature and Relative Humidty (RH) using the SCD41
*   Send data via ESP-NOW
*   Supports modem- and lightsleep
*   WiFi provisioning using SoftAP

To-do:

*   ESP-NOW: getting a peer's MAC-address dynamically on power-up
*   HTTPS: sending data to the API (smartphone app needed?)

## Status
Project is: _in progress_

## License
This software is available under the [Apache 2.0 license](./LICENSE.md), Copyright 2021 [Research group Energy Transition, Windesheim University of Applied Sciences](https://windesheim.nl/energietransitie) 

## Credits
We use and gratefully aknowlegde the efforts of the makers of the following source code and libraries:

* [ESP-IDF](https://github.com/espressif/esp-idf), by Copyright (C) 2015-2019 Espressif Systems, licensed under [Apache License 2.0](https://github.com/espressif/esp-idf/blob/9d34a1cd42f6f63b3c699c3fe8ec7216dd56f36a/LICENSE)
* [Twomes Generic ESP Firmware](https://github.com/energietransitie/twomes-generic-esp-firmware), by Copyright (C) 2021 Research group Energy Transition, Windesheim University of Applied Sciences, licensed under [Apache License 2.0](https://github.com/energietransitie/twomes-generic-esp-firmware/blob/b17f346d78ac7dde6f2dff6b5e7639e98d55c348/LICENSE.md)
* [Twomes Sensor Pairing](https://github.com/energietransitie/twomes-temp-monitor/blob/97c9b54c33c2031c82f80bd55b47af8e185d1a9a/src/twomes_sensor_pairing.h), by Copyright (C) 2021 Research group Energy Transition, Windesheim University of Applied Sciences, licensed under [Apache License 2.0](https://github.com/energietransitie/twomes-temp-monitor/blob/97c9b54c33c2031c82f80bd55b47af8e185d1a9a/LICENSE)
