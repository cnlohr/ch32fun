# HX711 Load Cell Amplifier Example

This example shows how to interface a HX711 load cell amplifier.
The project provides a simple interface for calibration and reading weight values over the debug interface.

## Hardware Connections

- **HX711 Data Pin:** Connect to `PD4` (can be changed using define `HX711_DATA_PIN`).
- **HX711 Clock Pin:** Connect to `PD5` (can be changed using define `HX711_CLK_PIN`).

## Usage

1. Use `make` to upload firmware and `make monitor` to open the terminal
1. On startup, the device will tare (zero) the scale.
2. Enter commands:
   - `C` + Enter: Starts calibration. Follow three prompts to place a known weight and enter its value.
   - `V` + Enter: Reads and displays the current weight value.