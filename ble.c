#include <SoftwareSerial.h>
#include <stdint.h>

#define TX_PIN 5  //D5 아두이노 핀
#define RX_PIN 4  //D4
#define RTS_PIN 3 //D3
#define CTS_PIN 2 //D2

#define DTR_PIN 6
#define DSR_PIN 7

#define DEFAULT_BAUD_RATE 115200
#define DEFAULT_BAUD_RATE_ID 8

#define BEACON_BUF_SIZE	128

SoftwareSerial bleSerial(RX_PIN, TX_PIN);  // RX, TX

uint8_t buf[BEACON_BUF_SIZE];
uint8_t request[10] = {0xFE, 0xFE, 0x05, 0x11, 0x02, 0x03, 0x00, 0x00, 0xFD, 0xFD};
uint8_t history_header[12] = {0xFE, 0xFE, 0x00, 0x00, 0x12, 0x02, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00}; //history_header[3] = size, history_header[10] = payload number
uint8_t history_tail[2] = {0xFD, 0xFD};

uint8_t payload[24] = {0x0F, 0x00, 0x11, 0x23, 0x00, 0x11, 0x00, 0x12, 0x01, 0x14, 0x01, 0x17, 0x09, 0x31, 0x06, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x00};

int idx = 0;

void setup() {  
  Serial.begin(DEFAULT_BAUD_RATE);
  while (!Serial) {}
  
  Serial.println();
  Serial.println("**************************************************************");
  Serial.println("*** BlueEva+Sxx Software serial port communication for BLE ***");
  Serial.println("**************************************************************");
  Serial.println();
  Serial.println("USB serial port configuration DONE!");

  // Pin configuration
  pinMode(RX_PIN, INPUT);   //4
  pinMode(TX_PIN, OUTPUT);  //5
  pinMode(CTS_PIN, INPUT);  //2
  pinMode(RTS_PIN, OUTPUT); //3

  // Pin initialization
  digitalWrite(RX_PIN, LOW); //4
  digitalWrite(TX_PIN, LOW); //5


  // Baud rate change
  bleSerial.begin(DEFAULT_BAUD_RATE);
  bleSerial.write("AT%B");
  bleSerial.println(DEFAULT_BAUD_RATE_ID);
  delay(10);
  bleSerial.begin(DEFAULT_BAUD_RATE);
  while (bleSerial.available() > 0) {    // Make empty the buffer of command response
    bleSerial.read();
  }
  memset((void *)buf, 0, (size_t)BEACON_BUF_SIZE);
  
  Serial.println("Ready to send AT commands to BLE!");
  Serial.println();
}

void loop() {
    while (bleSerial.available() > 0) {
      sprintf((char *)&buf[idx++], "%c", bleSerial.read());
    }
    if (!strncmp(buf, request, 10))
    {
      uint8_t history[38];
      memset((void *)history, 0, 38);
      sprintf((char *)&history[0], "%s", history_header);
      sprintf((char *)&history[12], "%s", payload);
      sprintf((char *)&history[36], "%s", history_tail);
      Serial.write("receive history request");
      bleSerial.write(history);
    }
    else
    {
      sprintf((char *)&buf[idx++], "%c", '\0');
      Serial.write(buf);
    }
    memset((void *)buf, 0, (size_t)BEACON_BUF_SIZE);
    idx = 0;

    while (Serial.available() > 0) {
      bleSerial.write(Serial.read());
    }
}