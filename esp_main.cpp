#include <esp_now.h>
#include <WiFi.h>

// Replace with the MAC Address of your receiving ESP32
uint8_t receiverMacAddress[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

// Callback when data is received
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
  String message = String((char*)incomingData);
  Serial.print("Received message: ");
  Serial.println(message);

  // Check if the received message is "HIGH"
  if (message.equals("HIGH")) {
    // Send a signal to the receiver ESP32 to turn on its built-in LED
    esp_now_send(receiverMacAddress, (uint8_t*) "HIGH", sizeof("HIGH"));
    Serial.println("Signal sent to turn on LED");
  }
}

void setup() {
  // Initialize Serial
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register peer (receiver ESP32)
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, receiverMacAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  // Register callback function for data received event
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // Check for messages from the laptop
  if (Serial.available() > 0) {
    String message = Serial.readStringUntil('\n');
    // Send the received message to the other ESP32
    esp_now_send(receiverMacAddress, (uint8_t*)message.c_str(), message.length());
    Serial.println("Message sent to the other ESP32: " + message);
  }

  // Code for the main loop if needed
  // ...
}
