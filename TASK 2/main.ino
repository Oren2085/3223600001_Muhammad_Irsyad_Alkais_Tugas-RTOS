#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>

// === Pin Definitions ===
#define LED1 16
#define LED2 4
#define BUZZER 2
#define BUTTON1 5
#define BUTTON2 36
#define BUTTON3 37
#define POT_PIN 35
#define SERVO_PIN 13
#define STEP_PIN 14
#define DIR_PIN 12
#define CLK 18
#define DT 17
#define SW 11
#define SDA_PIN 21
#define SCL_PIN 20

// === Global Variables ===
volatile int encoderPos = 0;
int lastCLKState;
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo myServo;

// === Task Handles ===
TaskHandle_t TaskCore0;
TaskHandle_t TaskCore1;

// === Encoder ISR ===
void IRAM_ATTR readEncoder() {
  int clkState = digitalRead(CLK);
  int dtState = digitalRead(DT);
  if (clkState != lastCLKState) {
    if (dtState != clkState) encoderPos++;
    else encoderPos--;
  }
  lastCLKState = clkState;
}

// === Core 0: Stepper + LED + Buzzer ===
void TaskStepperAndLED(void *pvParameters) {
  bool dirState = false;
  while (true) {
    // Stepper bolak-balik otomatis
    digitalWrite(DIR_PIN, dirState);
    for (int i = 0; i < 200; i++) {
      digitalWrite(STEP_PIN, HIGH);
      delayMicroseconds(700);
      digitalWrite(STEP_PIN, LOW);
      delayMicroseconds(700);
    }
    dirState = !dirState;

    // LED bergantian nyala
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, LOW);
    tone(BUZZER, 1000, 200);
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    digitalWrite(LED1, LOW);
    digitalWrite(LED2, HIGH);
    noTone(BUZZER);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

// === Core 1: Servo + Pot + Encoder + LCD ===
void TaskServoAndSensor(void *pvParameters) {
  int servoAngle = 0;
  bool increasing = true;

  while (true) {
    // Servo bergerak otomatis bolak-balik
    myServo.write(servoAngle);
    if (increasing) servoAngle += 5;
    else servoAngle -= 5;
    if (servoAngle >= 180) increasing = false;
    if (servoAngle <= 0) increasing = true;

    // Baca potensiometer
    int potValue = analogRead(POT_PIN);
    float voltage = (potValue / 4095.0) * 3.3;

    // Update LCD
    lcd.setCursor(0, 0);
    lcd.print("Volt: ");
    lcd.print(voltage, 2);
    lcd.print(" V   ");

    lcd.setCursor(0, 1);
    lcd.print("Encoder: ");
    lcd.print(encoderPos);
    lcd.print("   ");

    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);

  // === Pin Setup ===
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(BUTTON3, INPUT_PULLUP);
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);

  // === LCD & Servo Setup ===
  lcd.init();
  lcd.backlight();
  myServo.attach(SERVO_PIN);

  // === Encoder Setup ===
  lastCLKState = digitalRead(CLK);
  attachInterrupt(digitalPinToInterrupt(CLK), readEncoder, CHANGE);

  // === Task Creation ===
  xTaskCreatePinnedToCore(
    TaskStepperAndLED,
    "StepperLED",
    4096,
    NULL,
    1,
    &TaskCore0,
    0
  );

  xTaskCreatePinnedToCore(
    TaskServoAndSensor,
    "ServoSensor",
    4096,
    NULL,
    1,
    &TaskCore1,
    1
  );

  lcd.setCursor(0, 0);
  lcd.print("System Starting...");
  delay(1000);
  lcd.clear();
}

void loop() {
  // Kosong (semua dikendalikan oleh task)
}
