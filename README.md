## ESP-32 Internet of Things Projects

Under this repository, I will share some of the projects implemented with ESP-32 Development Kit. ESP-32 is a microprocessor with hybrid WiFi-Bluetooth Low Energy capabilities.

### Hardware

[ESP-32 Overview](https://www.espressif.com/en/products/hardware/esp32/overview)

[ESP-32 Shop Link](https://www.banggood.com/ESP32-Development-Board-WiFiBluetooth-Ultra-Low-Power-Consumption-Dual-Cores-ESP-32-ESP-32S-Board-p-1109512.html?rmmds=search&cur_warehouse=CN)

### Software Development

Software installed to the ESP-32 Dev. Kit is developed with Arduino IDE. Arduino IDE, after installing necessary packets, can be used for many other target MCUs.

[ESP-32 Development on Arduino IDE](https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/windows.md)

[USB Driver for ESP-32](https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers)

### What to Know Before

It is always an advantage having knowledge of introductory level embedded software development. Arduino is an open source design that can be used to produce many types of development boards. It has smoother learning curve compared to other platforms like Texas Instruments products, thanks to its open source community and high level abstraction provided by various libraries.

For this reason, Arduino IDE became really familiar with hobbyists. It can be configured to build binaries for many other target platforms. Espressif, the company behind ESP-32 also provides SDK but since it adds another layer of difficulty, we will use Arduino IDE to develop software.

We will use Wifi and Bluetooth Low Energy standards. Although ESP-32 libraries provide high level abstraction, it is always an advantage to have familiarity with those standards.

[Bluetooth Overview](https://learn.adafruit.com/introduction-to-bluetooth-low-energy/introduction)

[WiFi Architecture](http://www.tutorial-reports.com/wireless/wlanwifi/wifi_architecture.php)

The links above are provided for entry point but I suggest using multiple sources for wireless communication standards and embedded software developing. After having enough knowledge of both standards, I highly recommend try and test Bluetooth and WiFi examples in the Arduino IDE after installing ESP-32 packages.

