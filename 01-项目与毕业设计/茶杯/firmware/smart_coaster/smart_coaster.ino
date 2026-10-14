#include <Adafruit_GFX.h>
#include <Adafruit_MLX90614.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

namespace {
constexpr uint8_t kScreenWidth = 128;
constexpr uint8_t kScreenHeight = 32;
constexpr uint8_t kOledI2cAddress = 0x3C;
constexpr int8_t kOledResetPin = -1;

constexpr bool kShowFahrenheit = false;
constexpr float kCoolLimitC = 35.0f;
constexpr float kHotLimitC = 60.0f;
constexpr float kFilterWeight = 0.22f;
constexpr unsigned long kRefreshMs = 250;
constexpr unsigned long kSerialMs = 1000;
constexpr unsigned long kHeaterUpdateMs = 500;
constexpr uint8_t kMax30102Address = 0x57;
constexpr uint32_t kFingerThreshold = 30000;
constexpr uint16_t kPpgSampleRate = 100;
constexpr uint16_t kPpgBufferLength = 200;
constexpr uint16_t kSpo2MinSamples = 80;
constexpr int32_t kMinPpgAmplitude = 150;
constexpr uint16_t kMinBeatSamples = 25;
constexpr uint16_t kMaxBeatSamples = 250;
constexpr uint8_t kBeatAvgCount = 4;
constexpr uint8_t kHeaterOffPwm = 0;
constexpr uint8_t kHeaterFullPwm = 255;
constexpr float kHeaterOnBelowC = 40.0f;
constexpr float kHeaterOffAboveC = 42.0f;

#if defined(ARDUINO_ARCH_ESP32)
constexpr uint8_t kSdaPin = 6;
constexpr uint8_t kSclPin = 7;
#endif

constexpr uint8_t kHeaterStbyPin = 20;
constexpr uint8_t kHeaterAin1Pin = 4;
constexpr uint8_t kHeaterAin2Pin = 5;
constexpr uint8_t kHeaterPwmPin = 21;
}  // namespace

Adafruit_SSD1306 display(kScreenWidth, kScreenHeight, &Wire, kOledResetPin);
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

bool displayReady = false;
bool sensorReady = false;
bool ppgReady = false;
bool fingerPresent = false;
bool filterReady = false;
float filteredObjectC = 0.0f;
float filteredAmbientC = 0.0f;
uint8_t heaterPwm = 0;
int32_t heartRate = 0;
int8_t validHeartRate = 0;
int32_t spo2 = 0;
int8_t validSpo2 = 0;
uint32_t irBuffer[kPpgBufferLength] = {0};
uint32_t redBuffer[kPpgBufferLength] = {0};
int32_t acBuffer[kPpgBufferLength] = {0};
uint16_t ppgWriteIndex = 0;
uint16_t ppgSampleCount = 0;
uint32_t lastIrValue = 0;
float irDcAverage = 0.0f;
float irAcFiltered = 0.0f;
int32_t previousSignal = 0;
int32_t previousPreviousSignal = 0;
uint32_t ppgSampleNumber = 0;
uint32_t lastBeatSample = 0;
uint32_t lastCandidateSample = 0;
uint16_t beatBpms[kBeatAvgCount] = {0};
uint8_t beatBpmIndex = 0;
uint8_t beatBpmCount = 0;
int32_t ppgAmplitude = 0;
int32_t dynamicThreshold = 0;
bool signalWasBelowThreshold = true;
unsigned long lastRefreshMs = 0;
unsigned long lastSerialMs = 0;
unsigned long lastHeaterMs = 0;

float displayTemperature(float tempC) {
  return kShowFahrenheit ? (tempC * 9.0f / 5.0f + 32.0f) : tempC;
}

char displayUnit() {
  return kShowFahrenheit ? 'F' : 'C';
}

const __FlashStringHelper* temperatureState(float tempC) {
  if (tempC >= kHotLimitC) {
    return F("HOT");
  }
  if (tempC >= kCoolLimitC) {
    return F("WARM");
  }
  return F("COOL");
}

void showMessage(const __FlashStringHelper* line1,
                 const __FlashStringHelper* line2) {
  if (!displayReady) {
    return;
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 5);
  display.println(line1);
  display.setCursor(0, 18);
  display.println(line2);
  display.display();
}

void updateFilter(float objectC, float ambientC) {
  if (!filterReady) {
    filteredObjectC = objectC;
    filteredAmbientC = ambientC;
    filterReady = true;
    return;
  }

  filteredObjectC += (objectC - filteredObjectC) * kFilterWeight;
  filteredAmbientC += (ambientC - filteredAmbientC) * kFilterWeight;
}

void setHeaterPwm(uint8_t pwm) {
  heaterPwm = pwm;

  if (pwm == 0) {
    digitalWrite(kHeaterPwmPin, LOW);
    digitalWrite(kHeaterAin1Pin, LOW);
    digitalWrite(kHeaterAin2Pin, LOW);
    return;
  }

  digitalWrite(kHeaterStbyPin, HIGH);
  digitalWrite(kHeaterAin1Pin, HIGH);
  digitalWrite(kHeaterAin2Pin, LOW);
  if (pwm >= 255) {
    digitalWrite(kHeaterPwmPin, HIGH);
  } else {
    analogWrite(kHeaterPwmPin, pwm);
  }
}

void stopHeaterHard() {
  setHeaterPwm(0);
  digitalWrite(kHeaterStbyPin, LOW);
}

void configureHeater() {
  pinMode(kHeaterStbyPin, OUTPUT);
  pinMode(kHeaterAin1Pin, OUTPUT);
  pinMode(kHeaterAin2Pin, OUTPUT);
  pinMode(kHeaterPwmPin, OUTPUT);
  stopHeaterHard();
}

void updateHeater() {
  if (!sensorReady || !filterReady) {
    setHeaterPwm(kHeaterOffPwm);
    return;
  }

  if (heaterPwm > 0) {
    if (filteredObjectC >= kHeaterOffAboveC) {
      setHeaterPwm(kHeaterOffPwm);
    } else {
      setHeaterPwm(kHeaterFullPwm);
    }
    return;
  }

  if (filteredObjectC < kHeaterOnBelowC) {
    setHeaterPwm(kHeaterFullPwm);
  } else {
    setHeaterPwm(kHeaterOffPwm);
  }
}

void resetPpgReading() {
  fingerPresent = false;
  validHeartRate = 0;
  validSpo2 = 0;
  heartRate = 0;
  spo2 = 0;
  ppgWriteIndex = 0;
  ppgSampleCount = 0;
  lastIrValue = 0;
  irDcAverage = 0.0f;
  irAcFiltered = 0.0f;
  previousSignal = 0;
  previousPreviousSignal = 0;
  ppgSampleNumber = 0;
  lastBeatSample = 0;
  lastCandidateSample = 0;
  beatBpmIndex = 0;
  beatBpmCount = 0;
  ppgAmplitude = 0;
  dynamicThreshold = 0;
  signalWasBelowThreshold = true;
  for (uint8_t i = 0; i < kBeatAvgCount; i++) {
    beatBpms[i] = 0;
  }
}

bool i2cDevicePresent(uint8_t address) {
  Wire.beginTransmission(address);
  return Wire.endTransmission() == 0;
}

void max30102Write(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(kMax30102Address);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

uint8_t max30102Read8(uint8_t reg) {
  Wire.beginTransmission(kMax30102Address);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) {
    return 0;
  }

  Wire.requestFrom(kMax30102Address, static_cast<uint8_t>(1));
  if (Wire.available()) {
    return Wire.read();
  }
  return 0;
}

uint32_t max30102Read24() {
  uint32_t value = 0;
  for (uint8_t i = 0; i < 3; i++) {
    value <<= 8;
    if (Wire.available()) {
      value |= Wire.read();
    }
  }
  return value & 0x3FFFF;
}

bool readMax30102Sample(uint32_t* redValue, uint32_t* irValue) {
  Wire.beginTransmission(kMax30102Address);
  Wire.write(0x07);
  if (Wire.endTransmission(false) != 0) {
    return false;
  }

  const uint8_t bytesToRead = 6;
  if (Wire.requestFrom(kMax30102Address, bytesToRead) != bytesToRead) {
    return false;
  }

  *redValue = max30102Read24();
  *irValue = max30102Read24();
  return true;
}

uint8_t max30102AvailableSamples() {
  const uint8_t writePointer = max30102Read8(0x04) & 0x1F;
  const uint8_t readPointer = max30102Read8(0x06) & 0x1F;
  return (writePointer - readPointer) & 0x1F;
}

bool beginMax30102() {
  if (!i2cDevicePresent(kMax30102Address)) {
    return false;
  }

  max30102Write(0x09, 0x40);
  delay(100);
  max30102Write(0x04, 0x00);
  max30102Write(0x05, 0x00);
  max30102Write(0x06, 0x00);
  max30102Write(0x08, 0x1F); // FIFO Config (Sample Avg: 1, FIFO Rollover: true, FIFO Almost Full: 31)
  max30102Write(0x09, 0x03);
  max30102Write(0x0A, 0x27);
  max30102Write(0x0C, 0x3C);
  max30102Write(0x0D, 0x3C);
  return true;
}

void updateSpo2Estimate() {
  if (ppgSampleCount < kSpo2MinSamples) {
    validSpo2 = 0;
    return;
  }

  uint32_t redMin = UINT32_MAX;
  uint32_t redMax = 0;
  uint32_t irMin = UINT32_MAX;
  uint32_t irMax = 0;
  uint64_t redSum = 0;
  uint64_t irSum = 0;

  for (uint16_t i = 0; i < ppgSampleCount; i++) {
    redMin = min(redMin, redBuffer[i]);
    redMax = max(redMax, redBuffer[i]);
    irMin = min(irMin, irBuffer[i]);
    irMax = max(irMax, irBuffer[i]);
    redSum += redBuffer[i];
    irSum += irBuffer[i];
  }

  const float redDc = redSum / static_cast<float>(ppgSampleCount);
  const float irDc = irSum / static_cast<float>(ppgSampleCount);
  const float redAc = redMax - redMin;
  const float irAc = irMax - irMin;

  if (redDc <= 0 || irDc <= 0 || redAc < 100 || irAc < 100) {
    validSpo2 = 0;
    return;
  }

  const float ratio = (redAc / redDc) / (irAc / irDc);
  int32_t estimate = static_cast<int32_t>(110.0f - 25.0f * ratio);
  estimate = constrain(estimate, 70, 100);
  spo2 = estimate;
  validSpo2 = 1;
}

void updateHeartRateEstimate(int32_t acValue) {
  if (ppgSampleCount < 25) {
    validHeartRate = 0;
    return;
  }

  int32_t minAc = INT32_MAX;
  int32_t maxAc = INT32_MIN;
  for (uint16_t i = 0; i < ppgSampleCount; i++) {
    if (acBuffer[i] < minAc) {
      minAc = acBuffer[i];
    }
    if (acBuffer[i] > maxAc) {
      maxAc = acBuffer[i];
    }
  }

  ppgAmplitude = maxAc - minAc;
  if (ppgAmplitude < kMinPpgAmplitude) {
    validHeartRate = 0;
    previousSignal = acValue;
    signalWasBelowThreshold = true;
    return;
  }

  const int32_t threshold = minAc + (ppgAmplitude * 55L / 100L);
  dynamicThreshold = threshold;
  const uint32_t samplesSinceBeat = ppgSampleNumber - lastBeatSample;
  const uint32_t samplesSinceCandidate = ppgSampleNumber - lastCandidateSample;
  const bool crossedUp = signalWasBelowThreshold && acValue >= threshold &&
                         acValue > previousSignal;

  if (crossedUp && samplesSinceCandidate >= 15) {
    lastCandidateSample = ppgSampleNumber;
    if (lastBeatSample != 0 && samplesSinceBeat >= kMinBeatSamples &&
        samplesSinceBeat <= kMaxBeatSamples) {
      const uint16_t bpm =
          static_cast<uint16_t>((60UL * kPpgSampleRate) / samplesSinceBeat);
      if (bpm >= 35 && bpm <= 220) {
        beatBpms[beatBpmIndex] = bpm;
        beatBpmIndex = (beatBpmIndex + 1) % kBeatAvgCount;
        if (beatBpmCount < kBeatAvgCount) {
          beatBpmCount++;
        }

        uint16_t sum = 0;
        for (uint8_t i = 0; i < beatBpmCount; i++) {
          sum += beatBpms[i];
        }
        heartRate = sum / beatBpmCount;
        validHeartRate = 1;
      }
    }
    if (samplesSinceBeat >= kMinBeatSamples) {
      lastBeatSample = ppgSampleNumber;
    }
  }

  if (lastBeatSample != 0 && samplesSinceBeat > kMaxBeatSamples) {
    validHeartRate = 0;
    beatBpmCount = 0;
  }

  signalWasBelowThreshold = acValue < threshold;
  previousSignal = acValue;
}

void processPpgSample(uint32_t redValue, uint32_t irValue) {
  if (irValue < kFingerThreshold) {
    resetPpgReading();
    return;
  }

  if (!fingerPresent) {
    irDcAverage = static_cast<float>(irValue);
    irAcFiltered = 0.0f;
  }

  fingerPresent = true;
  ppgSampleNumber++;

  irDcAverage += (static_cast<float>(irValue) - irDcAverage) * 0.01f;
  const float acValue = static_cast<float>(irValue) - irDcAverage;
  irAcFiltered += (acValue - irAcFiltered) * 0.18f;
  const int32_t filteredAc = static_cast<int32_t>(irAcFiltered);

  redBuffer[ppgWriteIndex] = redValue;
  irBuffer[ppgWriteIndex] = irValue;
  acBuffer[ppgWriteIndex] = filteredAc;
  ppgWriteIndex = (ppgWriteIndex + 1) % kPpgBufferLength;
  if (ppgSampleCount < kPpgBufferLength) {
    ppgSampleCount++;
  }

  updateHeartRateEstimate(filteredAc);
  updateSpo2Estimate();
  lastIrValue = irValue;
}

void updatePpg() {
  if (!ppgReady) {
    return;
  }

  uint8_t samples = max30102AvailableSamples();
  while (samples > 0) {
    uint32_t redValue = 0;
    uint32_t irValue = 0;
    if (readMax30102Sample(&redValue, &irValue)) {
      processPpgSample(redValue, irValue);
    }
    samples--;
  }
}

void drawReading() {
  if (!displayReady) {
    return;
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print(F("Cup "));
  if (filterReady) {
    display.print(displayTemperature(filteredObjectC), 1);
    display.print(displayUnit());
  } else {
    display.print(F("--.-"));
  }

  display.setCursor(74, 0);
  display.print(F("Amb "));
  if (filterReady) {
    display.print(displayTemperature(filteredAmbientC), 0);
    display.print(displayUnit());
  } else {
    display.print(F("--"));
  }

  display.setCursor(0, 11);
  display.print(F("HR "));
  if (ppgReady && fingerPresent && validHeartRate) {
    display.print(heartRate);
  } else {
    display.print(F("--"));
  }
  display.print(F(" bpm"));

  display.setCursor(74, 11);
  display.print(F("O2 "));
  if (ppgReady && fingerPresent && validSpo2) {
    display.print(spo2);
    display.print('%');
  } else {
    display.print(F("--%"));
  }

  display.setCursor(0, 23);
  display.print(F("Heat AUTO H"));
  display.print(heaterPwm);

  display.display();
}

void printReading() {
  Serial.print(F("Cup: "));
  if (filterReady) {
    Serial.print(filteredObjectC, 2);
    Serial.print(F(" C, ambient: "));
    Serial.print(filteredAmbientC, 2);
    Serial.print(F(" C"));
  } else {
    Serial.print(F("-- C, ambient: -- C"));
  }
  Serial.print(F(", HR: "));
  if (ppgReady && fingerPresent && validHeartRate) {
    Serial.print(heartRate);
  } else {
    Serial.print(F("--"));
  }
  Serial.print(F(" bpm, SpO2: "));
  if (ppgReady && fingerPresent && validSpo2) {
    Serial.print(spo2);
    Serial.print('%');
  } else {
    Serial.print(F("--%"));
  }
  Serial.print(F(", Finger: "));
  Serial.print(fingerPresent ? F("YES") : F("NO"));
  Serial.print(F(", IR: "));
  Serial.print(lastIrValue);
  Serial.print(F(", Amp: "));
  Serial.print(ppgAmplitude);
  Serial.print(F(", Th: "));
  Serial.print(dynamicThreshold);
  Serial.print(F(", Heater PWM: "));
  Serial.print(heaterPwm);
  Serial.print(F(", Heater mode: auto 40C"));
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  configureHeater();
#if defined(ARDUINO_ARCH_ESP32)
  Wire.begin(kSdaPin, kSclPin);
#else
  Wire.begin();
#endif
  Wire.setClock(100000);

  displayReady = display.begin(SSD1306_SWITCHCAPVCC, kOledI2cAddress);
  if (displayReady) {
    display.setRotation(2);
    showMessage(F("Smart Coaster"), F("Starting..."));
  } else {
    Serial.println(F("OLED init failed. Check address and wiring."));
  }

  sensorReady = mlx.begin();
  if (!sensorReady) {
    Serial.println(F("MLX90614 init failed. Check sensor wiring."));
    showMessage(F("Smart Coaster"), F("IR sensor error"));
  }

  ppgReady = beginMax30102();
  if (ppgReady) {
    resetPpgReading();
  } else {
    Serial.println(F("MAX30102 init failed. Check wiring/power."));
    showMessage(F("Smart Coaster"), F("MAX30102 ERROR"));
  }

  showMessage(F("Smart Coaster"), F("Ready"));
  delay(700);
}

void loop() {
  updatePpg();

  const unsigned long now = millis();
  if (now - lastHeaterMs >= kHeaterUpdateMs) {
    lastHeaterMs = now;
    updateHeater();
  }

  if (now - lastRefreshMs >= kRefreshMs) {
    lastRefreshMs = now;
    if (sensorReady) {
      updateFilter(mlx.readObjectTempC(), mlx.readAmbientTempC());
    }
    drawReading();
  }

  if (now - lastSerialMs >= kSerialMs) {
    lastSerialMs = now;
    printReading();
  }
}
