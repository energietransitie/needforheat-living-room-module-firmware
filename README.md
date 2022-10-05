# Twutility/Twomes CO₂ and Bluetooth occupancy monitoring firmware
This repository contains the firmware source code and binary releases for the CO₂ and Bluetooth occupancy monitor.

This started under the Twomes project for use in residential buildings. It uses the [twomes-generic-esp-firmware](https://github.com/energietransitie/twomes-generic-esp-firmware) library to do Bluetooth presence detection and send measurements to a [Twomes server](https://github.com/energietransitie/twomes-backoffice-api). The current setup can be used in either residential or utility buildings.

![CO₂ and bluetooth occupancy monitor hardware](/twutility_m5coreink_scd41.jpg)

## Table of contents
* [General info](#general-info)
* [Deploying](#deploying)
* [Developing](#developing) 
* [Features](#features)
* [Status](#status)
* [License](#license)
* [Credits](#credits)

## General info
The Twomes CO₂ and Bluetooth occupancy monitor, in addition to [generic data sent by any Twomes measurement device](https://github.com/energietransitie/twomes-generic-esp-firmware#readme), sends data about the following additional properties via the [Twomes API](https://github.com/energietransitie/twomes-backoffice-api) to a Twomes server:

| Sensor | Property           | Unit | [Printf format](https://en.wikipedia.org/wiki/Printf_format_string) | Default measurement interval \[h:mm:ss\] | Description                            |
|--------|--------------------|------|--------|-------------------|----------------------------------------|
| [SCD41](https://sensirion.com/products/catalog/SCD41/)  | `CO2concentration` | ppm  | %u     | 0:10:00           | CO₂ concentration                      |
| [SCD41](https://sensirion.com/products/catalog/SCD41/)  | `relativeHumidity` | %RH  | %.1f   | 0:10:00           | Relative humidity                      |
| [SCD41](https://sensirion.com/products/catalog/SCD41/)  | `roomTemp`         | °C   | %.1f   | 0:10:00           | Air temperature                        |
| Bluetooth  | `countPresence`         | [-]   | %u   | 0:10:00           | number of smartphones responding to Bluetooth name request                        |

This data can be analyzed to determine changes in the ventilation ratio of a room, which enables research into the accuracy of learning the thermal characteristics of residential buildings via a dynamic heat balance model.

## Deploying
This section describes how you can deploy binary releases of the firmware, i.e. without changing the source code, without a development environment and without needing to compile the source code.

### Prerequisites
In addition to the [prerequisites described in the generic firmware for Twomes measurement devices](https://github.com/energietransitie/twomes-generic-esp-firmware#prerequisites), you need:
* an [M5Stack CoreInk](https://docs.m5stack.com/en/core/coreink).
* an [SCD41 sensor](https://sensirion.com/products/catalog/SCD41/) connected to the HAT connector of the M5Stack CoreInk. We used a [Seeed Grove SCD41 module](https://www.seeedstudio.com/Grove-CO2-Temperature-Humidity-Sensor-SCD41-p-5025.html) mounted inside a an [M5Stack Proto HAT](https://docs.m5stack.com/en/hat/hat-proto), connected as follows:
    
    | M5Stack CoreInk | SCD41 |
    |-----------------|-------|
    | GND             | GND   |
    | 5V out          | VCC   |
    | G25             | SDA   |
    | G26             | SCL   |

### Device preparation
See [Deploying section of the generic firmware for Twomes measurement devices](https://github.com/energietransitie/twomes-generic-esp-firmware#deploying).
The firmware needed can be found as a [release from this repository](https://github.com/energietransitie/twomes-co_2-sensor/releases).

## Developing
This section describes how you can change the source code using a development environment and compile the source code into a binary release of the firmware that can be deployed, either via the development environment, or via the method described in the section [Deploying](#deploying).

Please see the [developing section of the generic Twomes firmware](https://github.com/energietransitie/twomes-generic-esp-firmware#developing).

## Features
List of features ready and TODOs for future development (other than the [features of the generic Twomes firmware](https://github.com/energietransitie/twomes-generic-esp-firmware#features)). 

Ready:
* Measure properties according to the measurement table above. 
* Send the data collected to a Twomes server.
* Use [Twomes generic ESP32 firmware library](https://github.com/energietransitie/twomes-generic-esp-firmware).

To-do:
* Add support for the [M5Stack CO2 UNIT (SCD40)](https://docs.m5stack.com/en/unit/co2), or upcoming [M5Stack CO2L unit (SCD41)](https://twitter.com/M5Stack/status/1575074205900500993), connected via the CoreInk-HY2.0 4P Port.
* Add support for reading temperature value from a Si7051 as property `roomTemp` and rename the SCD41 temperature measurement `roomTempCO2`.

## Status
Project is: _in progress_

## License
This software is available under the [Apache 2.0 license](./LICENSE), Copyright 2022 [Research group Energy Transition, Windesheim University of Applied Sciences](https://windesheim.nl/energietransitie) 

## Credits
This software was created by:
* Nick van Ravenzwaaij · [@n-vr](https://github.com/n-vr)

Thanks also go to:
* Sjors Smit ·  [@Shorts1999](https://github.com/Shorts1999)
* Maarten Vermeulen · [@m44rtn](https://github.com/m44rtn)
* Tristan Jongedijk · [@tristanjongedijk](https://github.com/tristanjongedijk)
* Laurens de Boer · [@Laurenz02](https://github.com/Laurenz02)

Product owners:
* Henri ter Hofte · [@henriterhofte](https://github.com/henriterhofte) · Twitter [@HeNRGi](https://twitter.com/HeNRGi)
* Marco Winkelman · [@MarcoW71](https://github.com/MarcoW71)

We use and gratefully acknowlegde the efforts of the makers of the following source code and libraries:
* [ESP-IDF](https://github.com/espressif/esp-idf), by Espressif Systems, licensed under [Apache License 2.0](https://github.com/espressif/esp-idf/blob/9d34a1cd42f6f63b3c699c3fe8ec7216dd56f36a/LICENSE)
* [Twomes Generic ESP Firmware](https://github.com/energietransitie/twomes-generic-esp-firmware), by Research group Energy Transition, Windesheim University of Applied Sciences, licensed under [Apache License 2.0](https://github.com/energietransitie/twomes-generic-esp-firmware/blob/b17f346d78ac7dde6f2dff6b5e7639e98d55c348/LICENSE.md)
