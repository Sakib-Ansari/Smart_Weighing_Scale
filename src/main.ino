#include <Arduino.h>
#include <HX711.h>
#include <LiquidCrystal_I2C.h>
#include <SD.h>
#include <SPI.h>

// Pin definitions
#define HX711_DT 26
#define HX711_SCK 27
#define TARE_BUTTON 19
#define CALIBRATE_BUTTON 18
#define DIGIT_SHIFT_BUTTON 5
#define SET_OK_BUTTON 17
#define UNITS_BUTTON 16
#define DATA_LOG_BUTTON 4
#define SD_CS 15

// Unit conversion factors
enum Unit { MG, G, KG, LB, OZ };
const char* unitNames[] = {"mg", "g", "kg", "lb", "oz"};
const float unitFactors[] = {1000.0, 1.0, 0.001, 0.00220462, 0.03527396}; // Relative to grams
Unit currentUnit = G;

// HX711 and LCD objects
HX711 scale;
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Global variables
float weight = 0.0;
float calibrationFactor = 1.0; // Adjust during calibration
bool tareRequested = false;
bool calibrateMode = false;
bool logData = false;
int calibrationDigit = 0;
float knownWeight = 0.0;

// Debouncing variables
unsigned long lastButtonCheck = 0;
unsigned long lastLcdUpdate = 0;
const unsigned long debounceDelay = 300;
const unsigned long lcdUpdateInterval = 500;

void setup() {
  Serial.begin(115200);

  // Initialize HX711
  scale.begin(HX711_DT, HX711_SCK);
  scale.set_scale(calibrationFactor);
  scale.tare();

  // Initialize LCD
  lcd.init();
  lcd.backlight();

  // Initialize SD card
  if (!SD.begin(SD_CS)) {
    Serial.println("SD Card initialization failed!");
  }

  // Initialize button pins
  pinMode(TARE_BUTTON, INPUT_PULLUP);
  pinMode(CALIBRATE_BUTTON, INPUT_PULLUP);
  pinMode(DIGIT_SHIFT_BUTTON, INPUT_PULLUP);
  pinMode(SET_OK_BUTTON, INPUT_PULLUP);
  pinMode(UNITS_BUTTON, INPUT_PULLUP);
  pinMode(DATA_LOG_BUTTON, INPUT_PULLUP);
}

void loop() {
  unsigned long currentMillis = millis();

  // Read weight
  if (tareRequested) {
    scale.tare();
    tareRequested = false;
  }
  weight = scale.get_units(10); // Average of 10 readings

  // Handle buttons with debouncing
  if (currentMillis - lastButtonCheck >= debounceDelay) {
    if (digitalRead(TARE_BUTTON) == LOW) {
      tareRequested = true;
      lastButtonCheck = currentMillis;
    }
    if (digitalRead(CALIBRATE_BUTTON) == LOW) {
      calibrateMode = !calibrateMode;
      calibrationDigit = 0;
      knownWeight = 0.0;
      lastButtonCheck = currentMillis;
    }
    if (calibrateMode && digitalRead(DIGIT_SHIFT_BUTTON) == LOW) {
      calibrationDigit = (calibrationDigit + 1) % 4; // 4 digits for calibration
      lastButtonCheck = currentMillis;
    }
    if (calibrateMode && digitalRead(SET_OK_BUTTON) == LOW) {
      if (calibrationDigit == 3) { // Finalize calibration
        if (knownWeight > 0) {
          calibrationFactor = scale.get_units(10) / knownWeight;
          scale.set_scale(calibrationFactor);
          calibrateMode = false;
        }
      } else {
        knownWeight = knownWeight * 10 + 1; // Increment digit
      }
      lastButtonCheck = currentMillis;
    }
    if (digitalRead(UNITS_BUTTON) == LOW) {
      currentUnit = static_cast<Unit>((currentUnit + 1) % 5);
      lastButtonCheck = currentMillis;
    }
    if (digitalRead(DATA_LOG_BUTTON) == LOW) {
      logData = true;
      lastButtonCheck = currentMillis;
    }
  }

  // Update LCD
  if (currentMillis - lastLcdUpdate >= lcdUpdateInterval) {
    lcd.clear();
    if (calibrateMode) {
      lcd.setCursor(0, 0);
      lcd.print("Calib: ");
      lcd.print(knownWeight, 1);
      lcd.print(" g");
      lcd.setCursor(0, 1);
      lcd.print("Digit: ");
      lcd.print(calibrationDigit);
    } else {
      lcd.setCursor(0, 0);
      lcd.print("Weight: ");
      lcd.print(weight * unitFactors[currentUnit], 2);
      lcd.print(" ");
      lcd.print(unitNames[currentUnit]);
    }
    lastLcdUpdate = currentMillis;
  }

  // Log data to SD card
  if (logData) {
    File file = SD.open("/data.txt", FILE_APPEND);
    if (file) {
      file.print(currentMillis);
      file.print(",");
      file.print(weight * unitFactors[currentUnit], 2);
      file.print(",");
      file.println(unitNames[currentUnit]);
      file.close();
    } else {
      Serial.println("Error opening data.txt");
    }
    logData = false;
  }
}