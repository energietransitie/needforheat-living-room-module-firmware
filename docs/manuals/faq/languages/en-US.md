# Living Room Module - FAQ

Frequently asked information about the living room module.

# 1. What data does this device collect?

Every 10 minutes, the living room module measures:

- indoor temperature;
- CO₂ concentration;
- humidity;
- the number of occupants[^1] present.

[^1]: The device only counts the number of smartphone housemates that were registered via this device to be counted by this device; see also question 4.

## 2. How can I install the device?

For installation instructions, please refer to the [installation manual](../../installation/).

## 3. How can I re-connect the device to Wi-Fi?

A Wi-Fi reset of the device may be necessary if you change your home Wi-Fi network, if you want to connect the device to a different home Wi-Fi network, or for other reasons.

The Wi-Fi reset works as follows:

1. Hold down the button on top of the module (you will feel a distinct click).
2. After 10 seconds, you will see a green light near the button blink 5 times (in bright ambient light, you may not see it).
3. Release the button afterward.
4. The Info? Scan! QR code will be replaced by a QR code without a caption.
5. Scan this QR code with the NeedForHeat app and follow the instructions.

## 4. How can I register my smartphone and those of my housemates for occupancy detection?

For the research, device count the presence of occupants (i.e., you and your  housemates). The device does this by occasionally counting the number of smartphones present that belong to occupants. To do this, you need to register the smartphone of each occupant on the device at the beginning of the measurement period. This way, only occupants are counted (not neighbors). To register a smartphone for this purpose, follow these steps:

1. Use the black wheel with the button on the side (this will take you to the `SMARTPHONES:` screen).
2. Scroll down one line using the black wheel to select "+ toevoegen."
3. You will now see the `TOEVOEGEN?`` screen with instructions on what to do on the smartphone.
4. Proceed on the smartphone (either an iPhone or Android smartphone).
    1. Ensure that the smartphone is turned on and unlocked.
    2. Go to `Settings` > `Bluetooth`.
    3. Ensure that Bluetooth is enabled by toggling the switch.
    4. Your smartphone will now automatically search for available devices. Wait until `NeedForHeat_OK` appears in the list.
    5. Tap on `NeedForHeat_OK` to initiate the pairing.

5. A successful pairing will confirm the device with a clear beep, and the name of the added smartphone will be displayed on the screen of the measurement device.

If it doesn't work the first time, please try again.

You can always exit a menu on the screen by scrolling to `terug` using the black wheel on the side and selecting it by pressing the black wheel.

If you have added the wrong smartphone or no longer want it to be counted, you can remove it by scrolling to the name of the smartphone, selecting it (pressing the black wheel), and confirming the removal.

If you have not used the black wheel on the side for 2 minutes, the measurement device will automatically return to the normal state with the QR code labeled `Info? Scan!``

## 5. Does the smartphone remain connected to the device?

No, once a smartphone is paired with `NeedForHeat_OK` for the first time, the connection is immediately terminated. Once paired, devices are automatically counted, even without an active connection.

## 6. Does Bluetooth need to remain on throughout the entire measurement period on the registered smartphones?

Yes. You don't need to do anything else; detection and counting of presence are entirely automatic. Many people nowadays keep Bluetooth enabled by default; it has minimal impact on your smartphone's battery life.

## 7. What technology does this device use to detect presence?

This device uses [Bluetooth name requests to detect the presence of smartphones](https://github.com/energietransitie/twomes-generic-esp-firmware/blob/main/src/presence_detection/README.md#general-info).

## 8. Where can I find more technical information about this device?

The source code of this measurement device is publicly available on GitHub under [twomes-co2-occupancy-scd41-m5coreink-firmware](https://github.com/energietransitie/twomes-co2-occupancy-scd41-m5coreink-firmware).

## What if I have other quesions or remarks?
Please send an email to the NeedForHeat research helpdesk via [needforheatonderzoek@windesheim.nl](needforheatonderzoek@windesheim.nl).

