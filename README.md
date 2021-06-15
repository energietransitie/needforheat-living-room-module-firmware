# Twomes CO₂ meter
This repository contains the firmware and binary releases for two variants of CO₂ meters for the Twomes project, which differ only in the way they their send their data via the [Twomes API](https://github.com/energietransitie/twomes-backoffice-api) to a [Twomes server](https://github.com/energietransitie/twomes-backoffice-server), either directly or indirectly:
* a full-blown Twomes CO₂ measurement device that sends measurement data directly;
* a satellite Twomes CO₂ measurement device that sends its data indirectly, via a [Twomes P1-gateway](https://github.com/energietransitie/twomes-p1-port-logger-gateway);

## Table of contents
* [General info](#general-info)
* [Deploying](#deploying)
* [Developing](#developing) 
* [Features](#features)
* [Status](#status)
* [License](#license)
* [Credits](#credits)

## General info
This firmware is responsible for managing the CO₂ measurement device. It will gather measurement data on CO₂ concentration, temperature and humidty. 
The data gathered will be sent, either directly or via a gateway, via a [Twomes API](https://github.com/energietransitie/twomes-backoffice-api) to a [Twomes server](https://github.com/energietransitie/twomes-backoffice-server) and analyzed by a [Twomes analysis pipeline](https://github.com/energietransitie/twomes-analysis-pipeline) to determine changes in the ventilation ratio of a room. This enables research into the accuracy of learning the thermal characteristics of residential buildings via a dynamic heat balance model.

## Deploying

### Prerequisites
In addition to the [prerequisites described in the generic firmware for Twomes measurement devices](https://github.com/energietransitie/twomes-generic-esp-firmware#prerequisites), you need:
*   An [SCD41](https://www.sensirion.com/en/environmental-sensors/carbon-dioxide-sensors/carbon-dioxide-sensor-scd4x/) CO₂ sensor, connected via I²C to a device based on an ESP SoC;

### Deployment procedure
To deploy the any of the two firmware variants, please download a [release from this repository](https://github.com/energietransitie/twomes-co_2-sensor/releases) and proceed as indicated in the [deploying section of the generic firmware for Twomes measurement devices](https://github.com/energietransitie/twomes-generic-esp-firmware#deploying). However, at step 5 of the deploying process, please use the following command to flash the firmware to the device instead of the command given in the deploying section of the generic firmware:

```shell
esptool.py --chip esp32 --baud 460800 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 40m --flash_size detect 0x1000 bootloader.bin 0x8000 partitions.bin 0xe000 ota_data_initial.bin 0x10000 firmware.bin
```

## Developing

### Prerequisites
Prerequisites for [deploying](#deploying), plus:
*	[Visual Studio Code](https://code.visualstudio.com/download) installed
*	[PlatformIO for Visual Studio Code](https://platformio.org/install/ide?install=vscode) installed
*	this GitHub repository cloned

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

### Toggling the ESP-NOW code between sender and receiver
If you want to use a receiver to test this code, you can uncomment line 16 in `platformio.ini`: `#build_flags = -D ESP_NOW_RECEIVER`. Flash it to your chosen device and it will behave as a receiver.

### Changes necessary to get ESP-NOW working
To use the ESP-NOW code acting as the sender (i.e. the CO₂-measurement device), please make sure Bluetooth is disabled in `co2_sensor/sdkconfig` (`CONFIG_BT_ENABLED=n`). It is possbile to use the contents of `co2_sensor/sdkconfig.espnow`. 

ESP-NOW is configured by powering on the measurement device. After the device is powered on for the very first time, it will wait for the receiver to send, via ESP-NOW, its MAC-address and Wi-Fi channel. 

### Changes necessary to get HTTPS working
In order for HTTPS to work on your device please make sure to enable Bluetooth in `co2_sensor/sdkconfig` (`CONFIG_BT_ENABLED=y`). It is possible to use the contents of `co2_sensor/sdkconfig.https` and copy these to `co2_sensor/sdkconfig`.

you need to change something in the sdkconfig file.
In Visual Studio Code, go to 'Explorer' (CTRL + SHIFT + E). Then, navigate to the folder 'co2_sensor'. In this folder, search for a .txt file called 'sdkconfig.https'. Copy everything from this file (CTRL + A), then go to the .txt file 'sdkconfig'. In this file, remove everything and then paste the text you just copied from 'sdkconfig.https'. Now, you can use the CO₂ measurement device with HTTPS.

## Features
* Measure the following properties in a space using the [Sensirion SCD41 sensor](https://www.sensirion.com/en/environmental-sensors/carbon-dioxide-sensors/carbon-dioxide-sensor-scd4x/): 
  * CO₂ concentration (property name: `CO2concentration`);
  * temperature (property name: `roomTemp`) ;
  * relative rumidty (property name: `%RH`);
* as a satellite measurement device:
  * getting the MAC-address of the gateway dynamically on power-up;
  * send measurements via ESP-NOW to the gateway;
  * supports modem-, light- and deepsleep;
* as a full-blown Twomes measurement device 
  * Wi-Fi provisioning using BLE;
  * secure uploading of measurements to the API;

To-do:
* minor fixes for the CO₂ measurement device

## Status
Project is: _in progress_

## License
This software is available under the [Apache 2.0 license](./LICENSE), Copyright 2021 [Research group Energy Transition, Windesheim University of Applied Sciences](https://windesheim.nl/energietransitie) 

## Credits
This software is a collaborative effort of:
* Maarten Vermeulen · [@m44rtn](https://github.com/m44rtn)
* Tristan Jongedijk · [@tristanjongedijk](https://github.com/tristanjongedijk)
* Laurens de Boer · [@Laurenz02](https://github.com/Laurenz02)

Product owners:
* Marco Winkelman · [@MarcoW71](https://github.com/MarcoW71)
* Henri ter Hofte · [@henriterhofte](https://github.com/henriterhofte) · Twitter [@HeNRGi](https://twitter.com/HeNRGi)

We use and gratefully acknowlegde the efforts of the makers of the following source code and libraries:
* [ESP-IDF](https://github.com/espressif/esp-idf), by Espressif Systems, licensed under [Apache License 2.0](https://github.com/espressif/esp-idf/blob/9d34a1cd42f6f63b3c699c3fe8ec7216dd56f36a/LICENSE)
* [Twomes Generic ESP Firmware](https://github.com/energietransitie/twomes-generic-esp-firmware), by Research group Energy Transition, Windesheim University of Applied Sciences, licensed under [Apache License 2.0](https://github.com/energietransitie/twomes-generic-esp-firmware/blob/b17f346d78ac7dde6f2dff6b5e7639e98d55c348/LICENSE.md)
* [Twomes Sensor Pairing](https://github.com/energietransitie/twomes-temp-monitor/blob/97c9b54c33c2031c82f80bd55b47af8e185d1a9a/src/twomes_sensor_pairing.h), by Copyright (C) 2021 Research group Energy Transition, Windesheim University of Applied Sciences, licensed under [Apache License 2.0](https://github.com/energietransitie/twomes-temp-monitor/blob/97c9b54c33c2031c82f80bd55b47af8e185d1a9a/LICENSE)
