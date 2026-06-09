#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>
#include <Adafruit_GFX.h>
#include <Servo.h>

MCUFRIEND_kbv tft;

// TFT/Touch Pins
#define YP A1
#define XM A2
#define YM 7
#define XP 6
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// Servo on Pin 45 (Mega PWM)
#define SERVO_PIN 45
Servo myServo;

// Pulse limits for DS51150
const int MIN_PULSE = 600;  // Narrowed slightly to prevent slamming ends
const int MAX_PULSE = 2400; // Narrowed slightly for stability

int currentAngle = 90;
#define STEP_DELAY 15  // Slower steps reduce the "Current Spike"

#define BLACK 0x0000
#define WHITE 0xFFFF
#define GREEN 0x07E0
#define RED   0xF800

void setup() {
  uint16_t ID = tft.readID();
  tft.begin(ID);
  tft.setRotation(1);
  tft.fillScreen(BLACK);
  
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.print("HV Servo Stability Test");

  // Draw Buttons
  tft.fillRect(20, 80, 130, 100, RED);
  tft.fillRect(170, 80, 130, 100, GREEN);
  tft.drawRect(20, 80, 130, 100, WHITE);
  tft.drawRect(170, 80, 130, 100, WHITE);

  // CRITICAL: Attach and hold. Do not detach.
  myServo.attach(SERVO_PIN, MIN_PULSE, MAX_PULSE);
  myServo.write(currentAngle);
}

void loop() {
  TSPoint p = ts.getPoint();

  // Re-establish pin modes for the shield
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  pinMode(XP, OUTPUT);
  pinMode(YM, OUTPUT);

  if (p.z > 200 && p.z < 1000) {
    int x = map(p.y, 920, 120, 0, 320);
    int y = map(p.x, 150, 900, 0, 240);

    // FORWARD
    if (x > 170 && x < 300 && y > 80 && y < 180) {
      if (currentAngle < 180) {
        currentAngle++;
        myServo.write(currentAngle);
        delay(STEP_DELAY); // Slowing down the "Ask" prevents power sags
      }
    } 
    // BACK
    else if (x > 20 && x < 150 && y > 80 && y < 180) {
      if (currentAngle > 0) {
        currentAngle--;
        myServo.write(currentAngle);
        delay(STEP_DELAY);
      }
    }
  }
}