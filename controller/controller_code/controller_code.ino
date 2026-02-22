/**
 * @file    controller_code.ino
 * @brief   Control unit firmware for the "Nato Due Volte" carnival float
 *          (73rd edition of the Carnevale di Massafra).
 *
 * This sketch runs on a custom Arduino Mega shield and controls a scenic
 * robotic hand composed of 5 servo motors. Two identical units are used
 * (one per hand); they receive wireless commands from a shared remote
 * controller (see code_telecomando.ino) via the nRF24L01+ module.
 *
 * Communication protocol:
 *   Commands arrive as comma-separated strings "firstButton,secondButton".
 *   USE_FIRST determines which value this unit responds to:
 *     - Controller #1: set USE_FIRST to true
 *     - Controller #2: set USE_FIRST to false
 *
 * ┌─────────────────────────────────────────────────────────────────┐
 * │  IMPORTANT: set USE_FIRST correctly before flashing each board  │
 * └─────────────────────────────────────────────────────────────────┘
 *
 * Each command maps to a sequence of servo movements ("steps") grouped
 * into a Command structure. Steps with no pause (pause == 0) are grouped
 * and executed simultaneously; a non-zero pause value terminates a group
 * and introduces a delay before the next one begins.
 *
 * Hardware:
 *   - Arduino Mega 2560 (on custom shield)
 *   - nRF24L01+ transceiver module  (CE: pin 9, CSN: pin 53)
 *   - 5 servo motors                (signal pins: 4, 5, 6, 7, 8)
 *
 * Dependencies:
 *   - RF24 library (available via Arduino IDE Library Manager)
 */

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

// --- RF24 configuration ---
#define CE_PIN 9
#define CSN_PIN 53
RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "00001";

// --- Servo configuration ---
#define NUM_SERVOS 5
Servo servos[NUM_SERVOS];
int servoPins[NUM_SERVOS] = {4, 5, 6, 7, 8};

// ┌─────────────────────────────────────────────────────────────────┐
// │  Set to true for controller #1, false for controller #2         │
// └─────────────────────────────────────────────────────────────────┘
#define USE_FIRST false

// --- Data structures ---

/**
 * @brief A single movement instruction for one servo.
 *
 * @param servoIndex       Index of the target servo (0–4).
 * @param targetAngle      Target position in degrees.
 * @param time             Duration of the movement in milliseconds.
 * @param pause            Delay after this step before the next one (ms).
 *                         A non-zero value also terminates the current
 *                         synchronised group.
 * @param useInterpolation If true, the servo moves with linear interpolation
 *                         over the specified duration; if false, it jumps
 *                         directly to the target at full speed.
 */
struct Step {
  int servoIndex;
  int targetAngle;
  unsigned long time;
  unsigned long pause;
  bool useInterpolation;
};

/**
 * @brief A complete command, consisting of an ordered sequence of Steps.
 *
 * @param steps    Array of up to 20 steps.
 * @param numSteps Number of active steps in the array.
 * @param repeat   If true, the command loops until a new one is received.
 */
struct Command {
  Step steps[20];
  int numSteps;
  bool repeat;
};

// ---------------------------------------------------------------------------
// Command map — 7 commands (one per remote button, indexed 0–6)
// ---------------------------------------------------------------------------
Command commandMaps[7] = {
  // ---- Command 0: all servos to 0° (reset position) ----
  {
    {
      {0, 0, 2000, 0, true},  
      {1, 0, 2000, 0, true},  
      {2, 0, 2000, 0, true}, 
      {3, 0, 2000, 0, true},
      {4, 0, 2000, 10, true},
    },
    5,
    false
  },
  // ---- Command 1 ----
  {
    {
      {0,   0, 2000,  0, true},
      {1, 250, 1500,  0, true},
      {2,   0, 1500,  0, true},
      {3,   0, 1500,  0, true},  
      {4, 180, 2000, 10, true}
    },
    5,
    false
  },
  // ---- Command 2 ----
  {
    {
      {0,   0, 2000,   0, true},
      {1,   0, 2000,   0, true},
      {2, 260, 2000,   0, true},
      {3,  0,  2000,   0, true},
      {4,  0,  500,  10, true}
    },
    5,
    false
  },
  // ---- Command 3 (looping) ----
  {
    {
      {0, 140, 2000,  0, true},
      {1, 260, 2000,  0, true},
      {2, 260, 2000,  0, true},
      {3, 250, 2000,  0, true},
      {4, 180, 2000, 1000, true},
      {0, 0, 1500,    0, true},
      {1, 0, 1500,    0, true},
      {2, 0, 1500,    0, true},
      {3, 0, 1500,    0, true},
      {4, 0, 1500, 1000, true},
    },
    10,
    true
  },
  // ---- Comando 4 (looping) ----
  {
    {
      {0, 140,  2000, 0, true},
      {1, 260,  2000, 0, true},
      {2, 260,  2000, 0, true},
      {3, 250,  2000, 0, true},
      {4, 180,  2000, 1000, true},
      {0, 140,  1500, 0, true},
      {1, 200,  1500, 0, true},
      {2, 220,  1500, 0, true},
      {3, 210,  1500, 0, true},
      {4, 130,  1500, 1000, true},
    },
    10,
    true
  },
  // ---- Comando 5 (looping) ----
  {
    {
      {0, 0,   1200, 200, true},
      {1, 180, 1200, 200, true},
      {2, 0,   1200, 200, true},
      {3, 180, 1200, 200, true},
      {4, 90,  1000, 0, true}
    },
    5,
    true
  },
  // ---- Comando 6 (no interpolation - snap to position) ----
  {
    {
      {0, 30,  600, 100, false},
      {1, 60,  600, 100, false},
      {2, 120, 600, 100, false},
      {3, 150, 600, 100, false},
      {4, 180, 600, 0, false}
    },
    5,
    false
  }
};

// ---------------------------------------------------------------------------
// Global state
// ---------------------------------------------------------------------------
volatile int currentCommand = -1; // last received command index
volatile bool newCommand    = false;

// ---------------------------------------------------------------------------
// Radio helpers
// ---------------------------------------------------------------------------

/**
 * @brief Check the radio for an incoming command without blocking.
 *        Updates currentCommand and sets newCommand = true if a valid
 *        command is received. Safe to call during command execution.
 * @return true if a new command was received, false otherwise.
 */
static inline bool pollRadioAndUpdate() {
  if (radio.available()) {
    char text[32] = {0};
    radio.read(&text, sizeof(text));

    int firstNum = 0, secondNum = 0;
    sscanf(text, "%d,%d", &firstNum, &secondNum);
    int receivedNumber = USE_FIRST ? firstNum : secondNum;

    if (receivedNumber >= 0 && receivedNumber <= 6) {
      Serial.print("New comamnd received during execution: ");
      Serial.println(receivedNumber);
      currentCommand = receivedNumber;
      newCommand = true;
      return true;
    }
  }
  return false;
}

/**
 * @brief Blocking delay that can be interrupted by an incoming command.
 *        Returns early if a new command is detected via pollRadioAndUpdate().
 * @param ms Duration in milliseconds.
 */
static inline void cancellableDelay(unsigned long ms) {
  unsigned long endAt = millis() + ms;
  while ((long)(endAt - millis()) > 0) {
    // Se arriva un nuovo comando, usciamo subito
    if (pollRadioAndUpdate() || newCommand) return;
    delay(1);
  }
}

/** @brief Convenience macro: poll radio and return from caller if interrupted. */
#define CHECK_CANCEL() do { if (pollRadioAndUpdate() || newCommand) return; } while (0)

// ---------------------------------------------------------------------------
// Movement helpers
// ---------------------------------------------------------------------------

/**
 * @brief Internal state for one servo during a synchronised group movement.
 */
struct ServoMovement{
  int servoIndex;
  int startAngle;
  int targetAngle;
  unsigned long duration;
  bool useInterpolation;
  bool active;
};

// ---------------------------------------------------------------------------
// Command execution
// ---------------------------------------------------------------------------

/**
 * @brief Execute the command at the given index.
 *        Steps are grouped by their pause value: consecutive steps with
 *        pause == 0 are executed simultaneously; a step with pause > 0
 *        terminates the group and waits before the next group begins.
 *        The function returns immediately if a new command is received.
 * @param cmdIndex Index into commandMaps (0–6).
 */
void executeCommand(int cmdIndex) {
  Command cmd = commandMaps[cmdIndex];
  int loopNum = 0;

  do {
    int i = 0;
    while (i < cmd.numSteps){
      // --- Determine the extent of the current group ---
      int groupStart = i;
      int groupEnd = i;

      while (groupEnd < cmd.numSteps) {
        groupEnd++;
        if (groupEnd <= cmd.numSteps && cmd.steps[groupEnd - 1].pause != 0) {
          break;
        }
      }

      int groupSize = groupEnd - groupStart;

      // ---------------------------------------------------------------
      // Single-step group
      // ---------------------------------------------------------------
      if (groupSize == 1) {
        Step s = cmd.steps[i];

        Serial.print(">>> Single step ");
        Serial.print(i);
        Serial.print(": servo ");
        Serial.print(s.servoIndex);
        Serial.print(" -> ");
        Serial.print(s.targetAngle);
        Serial.print(" deg in ");
        Serial.print(s.time);
        Serial.println(" ms");

        if (s.useInterpolation){
          int startAngle = servos[s.servoIndex].read();
          unsigned long startTime = millis();

          while (millis() - startTime < s.time) {
            float progress = float(millis() - startTime) / s.time;
            int angle = startAngle + (s.targetAngle - startAngle) * progress;
            servos[s.servoIndex].write(angle);
            cancellableDelay(15);
            CHECK_CANCEL();
          }

          servos[s.servoIndex].write(s.targetAngle);
          Serial.println("Interpolated step done: " + String(i));
        } else {
          // snap to target at full servo speed
          servos[s.servoIndex].write(s.targetAngle);
          Serial.println("Natural step done: " + String(i));
          cancellableDelay(15); // allow the command to reach the servo
          CHECK_CANCEL();
        }
      
        if (s.pause > 0) {
            Serial.println(">>> Pause: " + String(s.pause) + " ms");
        }

        cancellableDelay(s.pause);
        if (newCommand) return;

        i++;
      // ---------------------------------------------------------------
      // Synchronised group (multiple servos moving in parallel)
      // ---------------------------------------------------------------
      } else {
        Serial.println("Executing synchronised group: steps  " + String(groupStart) + " to " + String(groupEnd - 1));
        
        ServoMovement movements[20];
        unsigned long maxDuration = 0;
        unsigned long finalPause = cmd.steps[groupEnd - 1].pause;
        bool anyInterpolated = false;
        
        // Prepare movement data for each servo in the group
        for (int j = 0; j < groupSize; j++) {
          Step s = cmd.steps[groupStart + j];
          movements[j].servoIndex = s.servoIndex;
          movements[j].startAngle = servos[s.servoIndex].read();
          movements[j].targetAngle = s.targetAngle;
          movements[j].duration = s.time;
          movements[j].useInterpolation = s.useInterpolation;
          movements[j].active = true;
          
          if (s.useInterpolation) {
            anyInterpolated = true;
            if (s.time > maxDuration) {
              maxDuration = s.time;
            }
          }
        }

        if (!anyInterpolated) {
          // All steps are natural: snap every servo simultaneously
          for (int j = 0; j < groupSize; j++) {
            servos[movements[j].servoIndex].write(movements[j].targetAngle);
          }
          Serial.println("Natural group executed instantly.");
          cancellableDelay(15);
        }
        else {
          // Drive all servos concurrently with per-servo interpolation
          unsigned long startTime = millis();
          unsigned long lastUpdate = 0;          

          while (millis() - startTime < maxDuration) {
            CHECK_CANCEL();
            
            unsigned long elapsed = millis() - startTime;
            
            if (elapsed - lastUpdate >= 15) {
              lastUpdate = elapsed;
            
              for (int j = 0; j < groupSize; j++) {
                if (movements[j].active) {
                  if (movements[j].useInterpolation) {
                    float progress = float(elapsed) / movements[j].duration;
                  
                    if (progress >= 1.0) {
                      servos[movements[j].servoIndex].write(movements[j].targetAngle);
                      movements[j].active = false;
                    } else {
                      int angle = movements[j].startAngle + 
                                 (movements[j].targetAngle - movements[j].startAngle) * progress;
                      servos[movements[j].servoIndex].write(angle);
                    }
                  } else {
                    // Natural step inside an interpolated group: snap and deactivate
                    servos[movements[j].servoIndex].write(movements[j].targetAngle);
                    movements[j].active = false;
                  }
                }
              }
            }
            cancellableDelay(1);
          }
          
          // Ensure every servo has reached its exact target
          for (int j = 0; j < groupSize; j++) {
            servos[movements[j].servoIndex].write(movements[j].targetAngle);
          }
          
          Serial.println("Synchronised mixed group complete.");          
        }

        cancellableDelay(finalPause);
        if (newCommand) return;

        i = groupEnd;
      }
    }

    loopNum++;
    Serial.println("Loop iteration: " + String(loopNum));
  } while (cmd.repeat && !newCommand);
  Serial.println("Command execution complete.");
}

// ---------------------------------------------------------------------------
// Setup & main loop
// ---------------------------------------------------------------------------

void setup(){
  Serial.begin(9600);

  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setChannel(110);
  radio.setCRCLength(RF24_CRC_16);
  radio.setAutoAck(true);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();

  for (int i = 0; i < NUM_SERVOS; i++){
    servos[i].attach(servoPins[i]);
    servos[i].write(0);
  }

  Serial.println("System ready.");
}

void loop(){
  if (radio.available()) {
    Serial.println("Receiving data...");
    char text[32] = {0};
    radio.read(&text, sizeof(text));
    
    int firstNum = 0, secondNum = 0;
    sscanf(text, "%d,%d", &firstNum, &secondNum);

    int receivedNumber = USE_FIRST ? firstNum : secondNum;

    if (receivedNumber >= 0 && receivedNumber <= 6){
      Serial.print("Command received: ");
      Serial.println(receivedNumber);
      currentCommand = receivedNumber;
      newCommand = true;
    }
  }

  if (newCommand && currentCommand != -1){
    newCommand = false;
    executeCommand(currentCommand);
  }
}