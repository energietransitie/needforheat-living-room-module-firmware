# Twomes CO2-sensor
This repository contains the software for the CO2-measurement devices. The software is based on ESP-IDF and is written for the ESP32.

## Table of contents
* [General info](#general-info)
* [Using binary releases](#using-binaries-releases)
* [Developing with the source code ](#developing-with-the-source-code) 
* [Features](#features)
* [Status](#status)
* [License](#license)
* [Credits](#credits)

## General info
This software is responsible for managing the CO2-measurement device it is loaded on. It will gather data on the CO2-concentration, temperature and humidty data. 

The data can be sent to the server in two ways. The first option is to send the data to the P1-gateway, which will send the data to the server. Another option would be that the data is sent to the server directly via Wi-Fi. 

The data gathered will be analyzed to see when a room is being ventilated. This makes it possible to give advice about the environmental friendliness of your home.

## Using binary releases
TODO

## Developing with the source code 
TODO

## Features
List of features ready and TODOs for future development. Ready:

* Measure CO2, temperature and Relative Humidty (RH) using the SCD41
* Send data via ESP-NOW

To-do:

* ESP-NOW: getting a peer's MAC-address dynamically on power-up

## Status
Project is: _in progress_

## License
This software is available under the [Apache 2.0 license](./LICENSE.md), Copyright 2021 [Research group Energy Transition, Windesheim University of Applied Sciences](https://windesheim.nl/energietransitie) 

## Credits
This software is a collaborative effort the following students and researchers:
* <contributor name 1> ·  [@Github_handle_1](https://github.com/<github_handle_1>) ·  Twitter [@Twitter_handle_1](https://twitter.com/<twitter_handle_1>)
* <contributor name 2> ·  [@Github_handle_2](https://github.com/<github_handle_2>) ·  Twitter [@Twitter_handle_2](https://twitter.com/<twitter_handle_2>)
* <contributor name 3> ·  [@Github_handle_3](https://github.com/<github_handle_3>) ·  Twitter [@Twitter_handle_3](https://twitter.com/<twitter_handle_3>)
* etc. 


We use and gratefully aknowlegde the efforts of the makers of the following source code and libraries:

* [ESP-IDF](https://github.com/espressif/esp-idf), by Copyright (C) 2015-2019 Espressif Systems, licensed under [Apache License 2.0](https://github.com/espressif/esp-idf/blob/9d34a1cd42f6f63b3c699c3fe8ec7216dd56f36a/LICENSE)
* [Twomes Generic ESP Firmware](https://github.com/energietransitie/twomes-generic-esp-firmware), by Copyright (C) 2021 Research group Energy Transition, Windesheim University of Applied Sciences, licensed under [Apache License 2.0](https://github.com/energietransitie/twomes-generic-esp-firmware/blob/b17f346d78ac7dde6f2dff6b5e7639e98d55c348/LICENSE.md)
