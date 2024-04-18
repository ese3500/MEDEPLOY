 // AARON ESP32 MAC ADDRESS:
 // EC:62:60:1D:11:20
 // DAILING ESP32 MAC ADDRESS:
 // CC:DB:A7:10:8E:10

// pin 2 and 12 need to be low for flashing
#include <esp_now.h>
#include <WiFi.h>

// REPLACE WITH THE MAC Address of your receiver 
uint8_t broadcastAddress[] = {0xCC,0xDB,0xA7,0x10,0x8E,0x10};

// Variable to store if sending data was successful
String success;

//Structure example to send data
//Must match the receiver structure
typedef struct struct_message {
    char msg[100];
    //int state;
    int p1;
    int p2;
    int p3;
    int door;
} struct_message;

struct_message outgoing;

struct_message incoming;
int in;

esp_now_peer_info_t peerInfo;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status ==0){
    success = "Delivery Success :)";
  }
  else{
    success = "Delivery Fail :(";
  }
}

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incoming, incomingData, sizeof(incoming));

  //Serial.print("Bytes received: ");
  //Serial.println(len);
  //in = incoming.msg;
  Serial.println("INCOMING MESSAGE");
  Serial.println(incoming.p1);
  Serial.println(incoming.p2);
  Serial.println(incoming.p3);
  Serial.println();

  /*if (incoming.state == 1) {
    digitalWrite(12, HIGH);
    digitalWrite(13, LOW);
  } else if (incoming.state == 2) {
    digitalWrite(12, LOW);
    digitalWrite(13, HIGH);
  } else {
    digitalWrite(12, LOW);
    digitalWrite(13, LOW);
  }*/
  if (incoming.p1 == 1) {
    digitalWrite(33, HIGH);
  } else {
    digitalWrite(33, LOW);
  }
  if (incoming.p2 == 1) {
    digitalWrite(13, HIGH);
  } else {
    digitalWrite(13, LOW);
  }
  if (incoming.p3 == 1) {
    digitalWrite(27, HIGH);
  } else {
    digitalWrite(27, LOW);
  }
  if (incoming.door == 1) {
    digitalWrite(14, HIGH);
  } else {
    digitalWrite(14, LOW);
  }
}
 
void setup() {
  Serial.begin(115200);
  
  pinMode(33, OUTPUT); // high for forward
  pinMode(13, OUTPUT); // high for backward
  pinMode(27, OUTPUT);
  pinMode(14, OUTPUT);
  // dont move when both low

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() {
  /*strcpy(outgoing.msg, "This is the LCD");
  
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &outgoing, sizeof(outgoing));
  
  if (result == ESP_OK) {
    //Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }

  delay(10000);*/
}
