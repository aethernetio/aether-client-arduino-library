# Æthernet for Arduino and ESP32

This library connects ESP32-based Arduino projects to Æthernet for secure real-time messaging, self-provisioning, and recovery on unstable networks.

[Documentation](https://aethernet.io/documentation) · [Tutorials](https://aethernet.io/tutorial) · [Examples](https://github.com/aethernetio/aethernet-examples)

## Supported environment

- ESP32 boards;
- Arduino IDE;
- Wi-Fi connectivity.

## Install from a ZIP file

1. Download this repository using **Code → Download ZIP**.
2. In Arduino IDE, choose **Sketch → Include Library → Add .ZIP Library**.
3. Open **File → Examples → Aether** and select an example.
4. Select your ESP32 board.
5. Add your Wi-Fi SSID and password where the example indicates.
6. Select a partition scheme with enough application space, such as **Minimal SPIFFS**, when required by your board configuration.
7. Build, upload, and open the serial monitor.

The screenshots in [`doc/pics`](doc/pics) show the same sequence in Arduino IDE.

## What the examples demonstrate

The included examples show device registration, connection to Æthernet, and message exchange. For multi-platform examples involving C++, Java, or TypeScript peers, use [aethernet-examples](https://github.com/aethernetio/aethernet-examples).

## Current limitations

- The library currently declares support for the `esp32` architecture.
- Installation is currently ZIP-based; the project is not yet documented as an Arduino Library Manager package.
- The compiled size depends on enabled logging, registration components, board core, and build configuration.

## Issues

When reporting a problem, include the ESP32 board, Arduino core version, Arduino IDE version, partition scheme, and relevant serial output.

## License

Apache License 2.0. See [LICENSE](LICENSE).
