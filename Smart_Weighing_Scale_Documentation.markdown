# Smart Weighing Scale Project Documentation

## Project Overview
The Smart Weighing Scale project is a microcontroller-based system designed to measure weight using a 50kg load cell and HX711 amplifier, display measurements on a 16x2 I2C LCD, log data to an SD card, and allow user interaction via six push buttons. The project is implemented on the Wokwi online simulator using an ESP32 microcontroller and programmed in the Arduino framework without FreeRTOS, ensuring a simple, sequential execution flow.

### Objectives
- Accurately measure weight using an HX711 load cell module.
- Support multiple weight units (mg, g, kg, lb, oz).
- Provide user controls for taring, calibration, unit switching, and data logging.
- Display weight and status on a 16x2 I2C LCD.
- Log weight measurements with timestamps to an SD card.
- Simulate the system in Wokwi for testing and demonstration.

## Components
The following components are used in the project:

| Component | Description | Role |
|-----------|-------------|------|
| **ESP32 DevKit V1** | Microcontroller with Wi-Fi and Bluetooth capabilities | Main processing unit |
| **HX711 with 50kg Load Cell** | 24-bit ADC and load cell for weight measurement | Measures weight |
| **Push Buttons (x6)** | Tare, Calibrate, Digit Shift, Set/OK, Units, Data Log | User input controls |
| **SD Card Module (SPI)** | Storage module for logging weight data | Logs measurements to a file |
| **16x2 I2C LCD** | 16x2 character display with I2C interface | Displays weight and status |

## Wiring Diagram
The components are wired to the ESP32 as defined in the Wokwi `diagram.json`. Below is a summary of the connections:

- **HX711**:
  - DT: ESP32 pin D26
  - SCK: ESP32 pin D27
  - VCC: ESP32 3.3V
  - GND: ESP32 GND
- **Push Buttons** (active-low, one side to GND):
  - Tare: ESP32 pin D19
  - Calibrate: ESP32 pin D18
  - Digit Shift: ESP32 pin D5
  - Set/OK: ESP32 pin D17
  - Units: ESP32 pin D16
  - Data Log: ESP32 pin D4
- **SD Card Module** (SPI):
  - MOSI: ESP32 pin D13
  - MISO: ESP32 pin D12
  - SCK: ESP32 pin D14
  - CS: ESP32 pin D15
  - VCC: ESP32 3.3V
  - GND: ESP32 GND
- **16x2 I2C LCD**:
  - SDA: ESP32 pin D21
  - SCL: ESP32 pin D22
  - VCC: ESP32 3.3V
  - GND: ESP32 GND
  - I2C Address: 0x27

The wiring is defined in the `diagram.json` file (artifact_id: `95da66c8-8f6f-46ee-9ece-4b1a335bdb1a`) for Wokwi simulation.

## Software Requirements
- **Platform**: Wokwi online simulator (ESP32 DevKit V1)
- **Programming Language**: C/C++ (Arduino framework)
- **Libraries**:
  - `HX711.h`: For load cell communication
  - `LiquidCrystal_I2C.h`: For LCD control
  - `SD.h` and `SPI.h`: For SD card operations
- **Dependencies**: Arduino core for ESP32 (included in Wokwi)

## Firmware Description
The firmware (`main.ino`, artifact_id: `e9db357c-4cc9-465e-b47f-766795f3436b`) is a non-FreeRTOS Arduino sketch that handles weight measurement, user input, LCD display, and data logging in a single `loop()` function.

### Key Features
- **Weight Measurement**: Reads weight from the HX711, averaging 10 readings for accuracy.
- **Taring**: Resets the scale to zero when the Tare button is pressed.
- **Calibration**: Allows users to input a known weight (digit-by-digit) to adjust the calibration factor.
- **Unit Switching**: Cycles through milligrams (mg), grams (g), kilograms (kg), pounds (lb), and ounces (oz).
- **Data Logging**: Appends weight, unit, and timestamp to `data.txt` on the SD card.
- **LCD Display**: Shows weight in the selected unit or calibration status, updated every 500ms.

### Code Structure
- **Global Variables**:
  - `weight`: Current weight reading (grams).
  - `calibrationFactor`: Scaling factor for HX711 readings.
  - `tareRequested`, `calibrateMode`, `logData`: Flags for user actions.
  - `calibrationDigit`, `knownWeight`: Calibration state variables.
  - `currentUnit`: Selected unit (enum: MG, G, KG, LB, OZ).
  - `unitNames`, `unitFactors`: Arrays for unit names and conversion factors.
  - `lastButtonCheck`, `lastLcdUpdate`: Timestamps for debouncing and LCD updates.
- **Setup**:
  - Initializes HX711, LCD, SD card, and button pins.
  - Sets initial calibration factor and tares the scale.
- **Loop**:
  - Reads weight from HX711.
  - Processes button inputs with 300ms debouncing using `millis()`.
  - Updates LCD every 500ms with weight or calibration status.
  - Logs data to SD card when requested.

### Calibration Process
1. Press **Calibrate** button to enter calibration mode.
2. Press **Digit Shift** to cycle through four digits (0-3).
3. Press **Set/OK** to increment the current digit (adds 1 to `knownWeight`).
4. After setting all digits, press **Set/OK** on the fourth digit to calculate and apply the calibration factor (`raw_reading / knownWeight`).
5. Calibration mode exits automatically.

### Data Logging
- When the **Data Log** button is pressed, the current weight, unit, and timestamp (millis()) are appended to `/data.txt` on the SD card.
- File format: `timestamp,weight,unit` (e.g., `123456,500.00,g`).

## Setup Instructions
1. **Create Wokwi Project**:
   - Start a new ESP32 DevKit V1 project in Wokwi.
   - Copy the `diagram.json` (artifact_id: `95da66c8-8f6f-46ee-9ece-4b1a335bdb1a`) into the project’s `diagram.json` file.
2. **Add Firmware**:
   - Copy the `main.ino` code (artifact_id: `e9db357c-4cc9-465e-b47f-766795f3436b`) into the project’s `main.ino` file.
3. **Install Libraries**:
   - In Wokwi’s "Libraries" tab, add:
     - `HX711`
     - `LiquidCrystal_I2C`
     - `SD` and `SPI` (built-in with Arduino).
4. **Simulate**:
   - Click the play button to start the simulation.
   - Use Wokwi’s HX711 interface to input simulated weight values.
   - Interact with buttons to test functionality.
   - View `data.txt` in Wokwi’s file explorer for logged data.

## Usage Instructions
- **Tare**: Press the Tare button to reset the scale to zero.
- **Calibrate**: Press the Calibrate button, then use Digit Shift and Set/OK to input a known weight. Finalize with Set/OK on the fourth digit.
- **Unit Switching**: Press the Units button to cycle through mg, g, kg, lb, oz.
- **Data Logging**: Press the Data Log button to save the current weight and unit to the SD card.
- **LCD Display**: Shows weight in the selected unit or calibration status (digit and known weight).

## Limitations
- **Wokwi HX711**: Requires manual weight input in the simulator, as physical load cells are not available.
- **Calibration Input**: Simplified to increment digits by 1 per Set/OK press; may need enhancement for precise input.
- **SD Card**: Wokwi simulates SD card operations, but ensure proper initialization to avoid errors.
- **Sequential Execution**: Without FreeRTOS, the `loop()` handles all tasks, which may slightly reduce responsiveness under heavy load.

## Future Improvements
- Enhance calibration input to allow full numeric entry (e.g., via multiple button presses).
- Add error handling for SD card failures (e.g., retry mechanism).
- Implement a low-power mode for real hardware deployment.
- Add a real-time clock (RTC) module for accurate timestamps in data logs.
- Optimize LCD refresh rate for smoother updates.

## Troubleshooting
- **No Weight Readings**: Check HX711 wiring and ensure weight input in Wokwi’s HX711 interface.
- **LCD Not Displaying**: Verify I2C address (0x27) and wiring (SDA: D21, SCL: D22).
- **SD Card Errors**: Ensure SD_CS pin (D15) is correct and the SD card is initialized.
- **Button Issues**: Confirm active-low configuration and debouncing logic.

## References
- [Wokwi Documentation](https://docs.wokwi.com/)
- [HX711 Library](https://github.com/bogde/HX711)
- [LiquidCrystal_I2C Library](https://github.com/johnrickman/LiquidCrystal_I2C)
- [Arduino SD Library](https://www.arduino.cc/en/Reference/SD)

## Author
Generated by Grok, created by xAI, based on user requirements.

## Last Updated
May 25, 2025