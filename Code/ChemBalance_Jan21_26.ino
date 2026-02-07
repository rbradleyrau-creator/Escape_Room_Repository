// Global Variable Declarations/Initializations
// Hall-effect Sensor inputs, named based of the corresponding object they are meant to detect
int oxygenPin = A1;
int nitrogenPin = A2;
int argonPin = A3;
int carbonDioxidePin = A4;
int oxygenVal = 0, nitrogenVal = 0, argonVal = 0, carbonDioxideVal = 0;

int solenoidLockPin = 14; // Pin that can unlock/lock the solenoid lock
int readDelay = 1000; // delay between reads, higher values delay lock response
int hallTrigSens = 1720; // ~1885 is roughly the reading on a Hall effect sensor with no magnet present
bool lockPrevTrig = false; // tracks if lcok has been triggered, prevents infinite unlocking/locking


// Runs once at the start of the program
void setup() {
  // Used for testing purposes
  Serial.begin(921600); 

  // Configure each hall effect sensor for input
  pinMode(oxygenPin, INPUT);
  pinMode(nitrogenPin, INPUT);
  pinMode(argonPin, INPUT);
  pinMode(carbonDioxidePin, INPUT);

  // Configure the pin connected to the solenoid lock relay for output
  pinMode(solenoidLockPin, OUTPUT);
}

void loop() {
  // Get a reading from each pin
  oxygenVal = analogRead(oxygenPin);
  nitrogenVal = analogRead(nitrogenPin);
  argonVal = analogRead(argonPin);
  carbonDioxideVal = analogRead(carbonDioxidePin);

  // If all sensors read less than the sensitivity and the lock hasn't been opened previously, then open the lock
  if ( (oxygenVal < hallTrigSens) && (nitrogenVal < hallTrigSens) &&
     (argonVal < hallTrigSens) && (carbonDioxideVal < hallTrigSens) && !lockPrevTrig) {

    openLock();
    lockPrevTrig = true;

  }
  
  // CODE FOR TESTING PURPOSES ONLY, COMMENT OUT WHEN NOT IN USE
  // Serial.print("Oxygen: ");
  // Serial.print(oxygenVal);
  // Serial.print(" | Nitrogen: ");
  // Serial.print(nitrogenVal);
  // Serial.print(" | Argon: ");
  // Serial.print(argonVal);
  // Serial.print(" | Carbon Dioxide: ");
  // Serial.println(carbonDioxideVal);

  // Within sensitivity
  // Serial.print("Oxygen: ");
  // Serial.print(oxygenVal < hallTrigSens);
  // Serial.print(" | Nitrogen: ");
  // Serial.print(nitrogenVal < hallTrigSens);
  // Serial.print(" | Argon: ");
  // Serial.print(argonVal < hallTrigSens);
  // Serial.print(" | Carbon Dioxide: ");
  // Serial.println(carbonDioxideVal < hallTrigSens);

  // END TEST CODE

  // Delay before attempting another read
  delay(readDelay);

}

// Opens the solenoid lock, then closes it after a 1.5 second delay
void openLock() {

  digitalWrite(solenoidLockPin, HIGH);
  delay(1500);
  digitalWrite(solenoidLockPin, LOW);

}
