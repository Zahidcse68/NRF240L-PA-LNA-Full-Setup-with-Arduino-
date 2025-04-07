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

void setup() {
  Serial.begin(9600);
  pinMode(4, INPUT_PULLUP);  // Forward button
  pinMode(5, INPUT_PULLUP);  // Reverse button
  pinMode(2, INPUT_PULLUP);  // Right button
  pinMode(3, INPUT_PULLUP);  // Left button

  radio.begin();
  printf_begin();
  radio.setPALevel(RF24_PA_HIGH);
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1, addresses[1]);
  radio.stopListening();

  Serial.println("Transmitter Ready");
}

void loop() {
  // Read buttons
  control.forward = !digitalRead(4);  // Active low
  control.reverse = !digitalRead(5);
  control.right = !digitalRead(2);
  control.left = !digitalRead(3);

  // Read joystick
  control.joystickX = analogRead(A7);  // X-axis
  control.joystickY = analogRead(A6);  // Y-axis

  // Send data
  if (radio.write(&control, sizeof(control))) {
    Serial.println("Data Sent");
  } else {
    Serial.println("Sending Failed");
  }

  delay(50);  // Small delay to avoid spamming
}
