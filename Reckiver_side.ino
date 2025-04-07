#include <SPI.h>
#include <RF24.h>
#include <printf.h>

RF24 radio(7, 8);  // CE, CSN
byte addresses[][6] = {"1Node", "2Node"};

struct ControlData {
  bool forward;
  bool reverse;
  bool right;
  bool left;
  int joystickX;
  int joystickY;
};

ControlData control;

const int ENA =4 ;
const int IN1 = 2;
const int IN2 = 3;
const int ENB = 10;
const int IN3 = 9;
const int IN4 = 6;

void setup() {
  Serial.begin(9600);
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  radio.begin();
  printf_begin();
  radio.setPALevel(RF24_PA_HIGH);
  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1, addresses[0]);
  radio.startListening();

  Serial.println("Receiver Ready");
}

void controlMotors(bool leftFwd, bool leftRev, bool rightFwd, bool rightRev, int speed) {
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);

  digitalWrite(IN1, leftFwd);
  digitalWrite(IN2, leftRev);
  digitalWrite(IN3, rightFwd);
  digitalWrite(IN4, rightRev);
}

void loop() {
  if (radio.available()) {
    radio.read(&control, sizeof(control));
    Serial.println("Data Received");

    int speed = map(abs(control.joystickY - 512), 0, 512, 0, 255);

    if (control.forward) {
      controlMotors(HIGH, LOW, HIGH, LOW, speed);
      Serial.println("Forward");
    } 
    else if (control.reverse) {
      controlMotors(LOW, HIGH, LOW, HIGH, speed);
      Serial.println("Reverse");
    } 
    else if (control.right) {
      controlMotors(HIGH, LOW, LOW, HIGH, speed);
      Serial.println("Right");
    } 
    else if (control.left) {
      controlMotors(LOW, HIGH, HIGH, LOW, speed);
      Serial.println("Left");
    } 
    else {
      int xVal = control.joystickX;
      int yVal = control.joystickY;

      if (xVal > 600) {
        controlMotors(HIGH, LOW, LOW, HIGH, speed);  // Turn Right
        Serial.println("Joystick Right");
      } 
      else if (xVal < 400) {
        controlMotors(LOW, HIGH, HIGH, LOW, speed);  // Turn Left
        Serial.println("Joystick Left");
      } 
      else if (yVal > 600) {
        controlMotors(HIGH, LOW, HIGH, LOW, speed);  // Forward
        Serial.println("Joystick Forward");
      } 
      else if (yVal < 400) {
        controlMotors(LOW, HIGH, LOW, HIGH, speed);  // Reverse
        Serial.println("Joystick Reverse");
      } 
      else {
        controlMotors(LOW, LOW, LOW, LOW, 0);  // Stop
        Serial.println("Stop");
      }
    }
  }
  delay(50);
}
