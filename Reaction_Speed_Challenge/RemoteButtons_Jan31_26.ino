// Includes
#include <espnow.h>
#include <ESP8266WiFi.h>

// Name defenitions
#define MY_NAME        "CONTROLLER_NODE_A"
#define MY_ROLE        ESP_NOW_ROLE_CONTROLLER  // set the role of this device: CONTROLLER, SLAVE, COMBO
#define RECEIVER_ROLE  ESP_NOW_ROLE_SLAVE       // The role of the receiver
#define WIFI_CHANNEL   1

uint8_t receiverAddress[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // Replaced with specific max address required.
const int loopDelay = 50; // Delay between check button presses (in milliseconds)
// Button input pins
int RED_IN_PIN = 14;
int BLU_IN_PIN = 12;

// Structure used to send data between two devices
struct __attribute__((packed)) dataPacket {
  int button;
};

// Outputs success/fail data to Serial regarding an attempted transmission
void transmissionComplete(uint8_t *receiver_mac, uint8_t transmissionStatus) {
  if (transmissionStatus == 0) {
    Serial.println("Data sent Successfully");
  }
  else {
    Serial.print("Error Code: ");
    Serial.println(transmissionStatus);
  }
}

void setup() {
  // Used for testing
  Serial.begin(921600);

  // Setup the buttons for input 
  pinMode(RED_IN_PIN, INPUT_PULLUP);
  pinMode(BLU_IN_PIN, INPUT_PULLUP);

  // BEGIN ESP-NOW SET-UP

  // Start and disconnect from WiFi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Check if ESP-NOW initialize correctly, end setup if failed
  if (esp_now_init() != 0) {
    Serial.println("ESP-NOW initialization failed");
    return;
  }

  esp_now_set_self_role(MY_ROLE);
  esp_now_register_send_cb(transmissionComplete); // this function is called once all data is sent
  esp_now_add_peer(receiverAddress, RECEIVER_ROLE, WIFI_CHANNEL, NULL, 0);

  Serial.println("Initialized.");

  // END ESP-NOW SETUP
}

void loop() {
  // Check if a button has been wait

  // Check if a button has been pressed and send pack if so
  if (digitalRead(RED_IN_PIN) == 0) {
    dataPacket packet;

    packet.button = 1;

    esp_now_send(receiverAddress, (uint8_t *) &packet, sizeof(packet));
  }

  if (digitalRead(BLU_IN_PIN) == 0) {
    dataPacket packet;
    
    packet.button = 3;

    esp_now_send(receiverAddress, (uint8_t *) &packet, sizeof(packet));
  }

  // For testing purposes only
  // Serial.print("Red Button: ");
  // Serial.print(digitalRead(RED_IN_PIN));
  // Serial.print("| Blue Button: ");
  // Serial.println(digitalRead(BLU_IN_PIN));

  delay(loopDelay);

}
