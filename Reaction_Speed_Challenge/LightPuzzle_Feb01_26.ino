// Includes
#include <espnow.h>
#include <ESP8266WiFi.h>

// Global Variables
// ENSURE YELLOW IS HIGHEST VALUE PIN AND GREEN IS LOWEST
int RED_OUT_PIN = 14; // button 1
int GRN_OUT_PIN = 12; // button 2
int BLU_OUT_PIN = 13; // button 3
int YEL_OUT_PIN = 15; // button 4
int rb, gb, bb, yb;

int START_IN_PIN = 2;
int RELAY_PIN = 4;
bool gameWon = false;

// PromptLed inputs global for dataReceived
bool allowInput = false;
bool rbCheck, gbCheck, bbCheck, ybCheck, correct;
int numSuccess, multFactor, totalLights;

// Name defenitions
#define MY_NAME   "SLAVE_NODE"

// Define what packets will look like when received
struct __attribute__((packed)) dataPacket {
  int button;
};

void dataReceived(uint8_t *senderMac, uint8_t *data, uint8_t datalength) {
  char macStr[18];
  dataPacket packet;

  // Converts the sender's mac address into printable
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", senderMac[0], senderMac[1], senderMac[2], senderMac[3], senderMac[4], senderMac[5]);
  
  Serial.println();
  Serial.print("Received data from: ");
  Serial.println(macStr);

  // Copy the transferred data into out packet
  memcpy(&packet, data, sizeof(packet));

  // Print which button was pressed (for Testing)
  // if (packet.button == 1) {
  //   Serial.println("Red Button input received.");
  // }

  // if (packet.button == 2) {
  //   Serial.println("Green Button input received.");
  // }

  // if (packet.button == 3) {
  //   Serial.println("Blue Button input received.");
  // }

  // if (packet.button == 4) {
  //   Serial.println("Yellow Button input received.");
  // }

  // Only execute if the game is allowing input
  if (allowInput) {

    // ----- CHECK EACH INPUT -----

    // Check for red button press
    if (packet.button == 1 && rbCheck) {

      rbCheck = false; // Prevents double testing with higher sample rates

      // Check if the unique pin for red is a part of the Product
      if ((multFactor % RED_OUT_PIN) == 0) {
        numSuccess++;
      }
      else {
        allowInput = false;
        return;
      }

    }

    // Check for green button press
    if (packet.button == 2 && gbCheck) {

      gbCheck = false; // Prevents double testing with higher sample rates

      // Check if the unique pin for green is a part of the Product
      if ((multFactor % GRN_OUT_PIN) == 0) {

        // Check to make sure that the previous required buttons have already been pressed
        // Check if Red Pin still needed to be pressed, end loop if so
        if (((multFactor % RED_OUT_PIN) == 0) && rbCheck) {
          allowInput = false;
          return;
        }

        numSuccess++;
      }
      else {
        allowInput = false;
        return;
      }

    }

    // Check for the blue button press
    if (packet.button == 3 && bbCheck) {

      bbCheck = false; // Prevents double testing with higher sample rates

      // Check if the unique pin for blue is a part of the Product
      if ((multFactor % BLU_OUT_PIN) == 0) {

        // Check to make sure that the previous required buttons have already been pressed
        // Check if Red Pin still needed to be pressed, end loop if so
        if (((multFactor % RED_OUT_PIN) == 0) && rbCheck) {
          allowInput = false;
          return;
        }
        // Check if Green Pin still needed to be pressed, end loop if so
        if (((multFactor % GRN_OUT_PIN) == 0) && gbCheck) {
          allowInput = false;
          return;
        }

        numSuccess++;
      }
      else {
        allowInput = false;
        return;
      }

    }

    // Check for yellow button press
    if (packet.button == 4 && ybCheck) {

      ybCheck = false; // Prevents double testing with higher sample rates

      // Check if the unique pin for yellow is a part of the Product
      if ((multFactor % YEL_OUT_PIN) == 0) {

        // Check to make sure that the previous required buttons have already been pressed
        // Check if Red Pin still needed to be pressed, end loop if so
        if (((multFactor % RED_OUT_PIN) == 0) && rbCheck) {
          allowInput = false;
          return;
        }
        // Check if Green Pin still needed to be pressed, end loop if so
        if (((multFactor % GRN_OUT_PIN) == 0) && gbCheck) {
          allowInput = false;
          return;
        }
        // Check if Green Pin still needed to be pressed, end loop if so
        if (((multFactor % BLU_OUT_PIN) == 0) && bbCheck) {
          allowInput = false;
          return;
        }

        numSuccess++;
      }
      else {
        allowInput = false;
        return;
      }

    }

    if (numSuccess == totalLights) {
      correct = true;
      allowInput = false;
      return;
    }

  }
}

void setup() {
  // Print the Initialization and Mac address (for testing)
  // Serial.println("");
  // Serial.println("");
  // Serial.println("");
  // Serial.print("Initializing...");
  // Serial.println(MY_NAME);
  // Serial.print("Mac Address is: ");
  // Serial.println(WiFi.macAddress());

  // Start and disconnect from WiFi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Check if ESP-NOW initialize correctly, prematurely end setup if not
  if (esp_now_init() != 0) {
    Serial.println("ESP-NOW initialization failed");
    return;
  }

  esp_now_register_recv_cb(dataReceived);

  Serial.println("Initialized.");

  // Setup pins for input/output using a loop
  for (int i = GRN_OUT_PIN; i <= YEL_OUT_PIN; i++) {
    pinMode(i, OUTPUT);
  }
  // Configure start button
  pinMode(START_IN_PIN, INPUT_PULLUP);
  // Configure Relay
  pinMode(RELAY_PIN, OUTPUT);
}

void loop() {

  // Start the puzzle sequence if the start button is pressed
  int val = digitalRead(START_IN_PIN);

  if (!val) {

    if (gameWon) {
      // Once game has been won, reset button only unlocks the cabinet
      unlock();
    }
    else {
      startPuzzle();
    }
  }

}

// Starts the challnege
void startPuzzle() {

  // Variable declarations/initializations
  const int ROUND_DELAY = 800;
  bool correct = true;
  int numLoops = 0;

  // Loop while the user gets the sequences correct
  while (correct) {
    // Start with a delay between rounds
    delay(ROUND_DELAY);

    // Perform user prompt with an increasing number of LED's to match
    correct = promptLED(1 + (numLoops / 3));

    // Check if the game has been completed
    if (numLoops == 9) {
      break;
    }

    // Increment loop counter
    numLoops++;
  }

  // Short delay on end
  delay(300);

  // Check if the player has won or not
  if (correct) {
    puzSuccess();
  } 
  else {
    puzFail();
  }

}

// prompt a number of LEDs for response
bool promptLED(int numLights) {
  
  // Values to be changed for desired results 
  const int RESPONSE_TIME = 2000; // In milliseconds
  const int SAMPLES_PER_SECOND = 500;

  // Assume all buttons to not have been pressed
  rb = HIGH;
  gb = HIGH;
  bb = HIGH;
  yb = HIGH;

  // Assume all buttons need to be checked 
  // Set to false after one check to prevent a double check for the same press
  rbCheck = true, gbCheck = true, bbCheck = true, ybCheck = true;

  // Variable Declarations/Initializations
  correct = false; // Assume the user did not respond correctly by default
  numSuccess = 0;
  totalLights = numLights;
  float sampleDelay = 1000.0 / SAMPLES_PER_SECOND;
  float milliSecs = 0.0;
  int leds[numLights];
  // The Product of each unique pin mulitplied together, 
  // allows for quick testing if a pin is included in the array
  multFactor = 1; 

  // Assign each element of leds a unique led, and turn it on
  for (int i = 0; i < numLights; i++) {

    bool unique;
    do {

      // Assume to be unique by default
      unique = true;

      // Assign the element of the array a Pin
      leds[i] = random(GRN_OUT_PIN, YEL_OUT_PIN + 1);

      // Check if it is a unique pin
      for (int j = 0; j < i; j++) {
        
        if (leds[j] == leds[i]) {
          unique = false;
          break;
        }

      }

    } while (!unique);

    digitalWrite(leds[i], HIGH);
    multFactor *= leds[i];
  }

  // Allow input to be recieved
  allowInput = true;

  // Loop to allow time for buttons to be pressed
  do {

    // Delay to allow for the specified number of samples per second
    delay(sampleDelay);
    milliSecs += sampleDelay;

  } while ( (milliSecs < RESPONSE_TIME) && allowInput);
  
  // Disallow input to be recieved after the round ends, if it hasn't been disabled already
  allowInput = false;

  // Turn all the LEDs off
  for (int i = 0; i < numLights; i++) {
    digitalWrite(leds[i], LOW);
  }

  // return the result of the call
  return correct;

}

// Blink all LEDs 3 times on a failed attempt
void puzFail() {

  // delay between blinks
  int blinkDelay = 175;

  // Loop 3 times
  for (int i = 0; i < 3; i++) {

    // Wait
      delay(blinkDelay);

    // Loop through each LED and turn them on
    for (int i = GRN_OUT_PIN; i <= YEL_OUT_PIN; i++) {
      digitalWrite(i, HIGH);
    }

    // Wait
    delay(blinkDelay);

    // Loop through each LED and turn them off
    for (int i = GRN_OUT_PIN; i <= YEL_OUT_PIN; i++) {
      digitalWrite(i, LOW);
    }


  }

}

// Turn on all LEDs for 3 seconds on a successful attempt, # FIXME open compartment
void puzSuccess() {
  int lightLength = 750;

  // Loop through each LED and turn them on
  for (int i = GRN_OUT_PIN; i <= YEL_OUT_PIN; i++) {
    digitalWrite(i, HIGH);
  }

  // Wait
  delay(lightLength);

  // Loop through each LED and turn them off
  for (int i = GRN_OUT_PIN; i <= YEL_OUT_PIN; i++) {
    digitalWrite(i, LOW);
  }

  delay(100);

  // Loop through each LED and turn them on
  for (int i = GRN_OUT_PIN; i <= YEL_OUT_PIN; i++) {
    digitalWrite(i, HIGH);
  }

  // Wait
  delay(lightLength);

  // Loop through each LED and turn them off
  for (int i = GRN_OUT_PIN; i <= YEL_OUT_PIN; i++) {
    digitalWrite(i, LOW);
  }

  gameWon = true;
  unlock();
}

// unlocks cabinet for 1.5s
void unlock() {

  digitalWrite(RELAY_PIN, HIGH);
  delay(1500);
  digitalWrite(RELAY_PIN, LOW);
  
}
