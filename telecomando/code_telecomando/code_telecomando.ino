/** 
 * @file    code_telecomando.ino
 * @brief   Remote controller firmware for the "Nato Due Volte" carnival float
 *          (73rd edition of the Carnevale di Massafra).
 *
 * This sketch runs on a custom Arduino Nano shield and handles input from
 * seven physical buttons. Button presses are transmitted wirelessly via
 * the nRF24L01+ module to two independent control units, each driving a
 * scenic robotic hand (5 servo motors per hand).
 *
 * Communication protocol:
 *   A command is composed of two sequential button presses sent as a
 *   comma-separated string "firstButton,secondButton" (e.g. "0,3").
 *   The first press selects the command for controller #1 (USE_FIRST = true),
 *   the second press selects the command for controller #2 (USE_FIRST = false).
 *   A 2-second timeout resets the sequence if the second press is not received.
 *
 * Hardware:
 *   - Arduino Nano (on custom shield)
 *   - nRF24L01+ transceiver module
 *   - 7 push buttons (indexed 0–6, left to right, top to bottom)
 *
 * Dependencies:
 *   - RF24 library (available via Arduino IDE Library Manager)
 */

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN 9
#define CSN_PIN 10
RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "00001";

#define NUM_BUTTONS 7
int buttonPins[NUM_BUTTONS] = {2, 3, 4, 5, 6, 7, 8};

int lastState[NUM_BUTTONS];

int firstPressed = -1;
unsigned long firstPressTime = 0;
const unsigned long TIMEOUT = 2000; // ms - max interval between the two button presses

void setup() {
  Serial.begin(9600);
  
  for (int i = 0; i < NUM_BUTTONS; i++) {
    pinMode(buttonPins[i], INPUT);
    lastState[i] = LOW; // initial state: not pressed
  }

  radio.begin();
  radio.openWritingPipe(address);
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(110);
  radio.setCRCLength(RF24_CRC_16);
  radio.setAutoAck(true);
  radio.setRetries(5, 15);
  radio.setPALevel(RF24_PA_LOW);
  radio.stopListening(); // transmit mode

  Serial.println("Remote controller ready.");
}

void loop() {
  // Reset first press if timeout has elapsed
  if (firstPressed != -1 && (millis() - firstPressTime > TIMEOUT)){
    Serial.println("Timeout: first press reset.");
    firstPressed = -1;
  }

  for (int i = 0; i < NUM_BUTTONS; i++) {
    int state = digitalRead(buttonPins[i]);

    // Rising edge detected (button just pressed)
    if (state == HIGH && lastState[i] == LOW) {
      if (firstPressed == -1){
        // Record first press
        firstPressed = i;
        firstPressTime = millis();
        Serial.print("First press: button ");
        Serial.println(i);
      } else {
        // Second press: build and transmit command string
        char message[16];
        snprintf(message, sizeof(message), "%d,%d", firstPressed, i);
        radio.write(&message, sizeof(message));
        Serial.print("Command sent: ");
        Serial.println(message);

        firstPressed = -1; // reset for next command
      }
      delay(200); // debounce
    }
    lastState[i] = state;
  }
}
