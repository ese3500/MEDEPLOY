 // AARON ESP32 MAC ADDRESS:
 // EC:62:60:1D:11:20
 // DAILING ESP32 MAC ADDRESS:
 // CC:DB:A7:10:8E:10

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
    int pain;
    int help;
    int bpm;
} struct_message;

struct_message outgoing;

struct_message incoming;
String in;

esp_now_peer_info_t peerInfo;

//atmega pins
int pinLSB = 15;
int pinMB = 27;
int pinMSB = 33;
int qnum = 1;
bool waiting = true;

int pain = 0;
int help = 0;


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
  Serial.println(incoming.msg);
}
 
void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(pinLSB, INPUT);
  pinMode(pinMB, INPUT);
  pinMode(pinMSB, INPUT);
  //delay(10000);
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(1000);                      // wait for a second
  digitalWrite(LED_BUILTIN, LOW);
  
  


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

  
  
  // read the input pin:
  int LSB = digitalRead(pinLSB);
  int MB = digitalRead(pinMB);
  int MSB = digitalRead(pinMSB);
  

  // print out the state of the button:
  int state = 4*MSB + 2*MB + LSB;

  
  if (state != 7 && waiting){
    waiting = false; 
    if (qnum == 1){
      Serial.println("Pain rating: ");
      Serial.print(state);
      outgoing.pain = state;
    }
    else if (qnum == 2){
      Serial.println("Help needed: ");
      Serial.print(state);
      outgoing.help = state;

      
      
      while (true){
        //delay(100);
        LSB = digitalRead(pinLSB);
        MB = digitalRead(pinMB);
        MSB = digitalRead(pinMSB);
        int bit3 = 0;

        // print out the state of the button:
        int state = 4*MSB + 2*MB + LSB;

        bit3 = state;
        Serial.print("state: ");
        Serial.println(state);
        delay(2000);
        int bpm = 32*MSB + 16*MB + 8*LSB + bit3;
        if (bpm == 0){
          Serial.println("No pulse detected");
        }else{
          Serial.print("bpm");
          Serial.println(bpm);
          outgoing.bpm = bpm; //for now

          while (true){
            //send
          strcpy(outgoing.msg, "This is the module");
          // Send message via ESP-NOW
          esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &outgoing, sizeof(outgoing));
  
          if (result == ESP_OK) {
            Serial.println("Sent with success");
          }
          else {
            Serial.println("Error sending the data");
          }
          }
          
      

        }
        
        delay(2000);
      }
      

    }
    qnum++;
  }
  if (state == 7){
    waiting = true;
  }
  
  //Serial.println(state);
  delay(10);  // delay in between reads for stability
  
  
}