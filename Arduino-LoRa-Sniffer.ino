// MIT License 
// Copyright 2017 LooUQ Incorportated.
// Many thanks to Adafruit and AirSpayce.

/* Example sketch to capture all packets on a LoRa (RF95) radio network and send them to the      
 *  serial port for monitoring or debugging a LoRa network.  
 *  
 *  Output MODE can be verbose or delimited
 *      - verbose sends output as readable text output with each field labeled
 *      - delimited sends output as a compressed text string, delimited by a char of your choice
 * Delimited is intended to be captured to a file on the serial host and displayed in an application
 * such as Excel or OpenOffice
 */

enum OutputMode { verbose, delimited };

/* Define your desired output format here */
OutputMode mode = delimited;
#define DELIMETER_CHAR '~'
/* end output format defintion */

#include <SPI.h>
#include <RH_RF95.h>
#include <stdio.h>

#define RH_FLAGS_ACK 0x80

/* for feather32u4
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7
*/

/* for feather m0  */
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3

/* for shield 
#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 7
*/

/* Feather 32u4 w/wing
#define RFM95_RST     11   // "A"
#define RFM95_CS      10   // "B"
#define RFM95_INT     2    // "SDA" (only SDA/SCL/RX/TX have IRQ!)
*/

/* Feather m0 w/wing 
#define RFM95_RST     11   // "A"
#define RFM95_CS      10   // "B"
#define RFM95_INT     6    // "D"
*/

/* Teensy 3.x w/wing 
#define RFM95_RST     9   // "A"
#define RFM95_CS      10   // "B"
#define RFM95_INT     4    // "C"
*/

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// Blink LED on packet receipt (Adafruit M0 here)
#define LED 13
 

int16_t rxCount = 0;                                        // packet counter
uint8_t rxBuffer[RH_RF95_MAX_MESSAGE_LEN];                  // receive buffer
uint8_t rxRecvLen;                                          // number of bytes actually received
char printBuffer[512] = "\0";                               // to send output to the PC
char formatString[] = {"%d~%d~%d~%d~%d~%d~%2x~%s~%d~%s"};
char legendString[] = "Rx Count~Rx@millis~LastRSSI~FromAddr~ToAddr~MsgId~HdrFlags~isAck~PacketLen~PacketContents";

void setup() 
{
    pinMode(LED, OUTPUT);     
    pinMode(RFM95_RST, OUTPUT);
    digitalWrite(RFM95_RST, HIGH);

    while (!Serial);
    Serial.begin(9600);
    delay(100);

  Serial.print("Feather LoRa Network Probe [Mode=");
  Serial.print(mode == verbose ? "verbose" : "delimeted");
  Serial.println("]");
  
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  if (!rf95.setFrequency(RF95_FREQ)) {                  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
      Serial.println("setFrequency failed");
      while (1);                                        // if can't set frequency, we are cooked!
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

    // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

    // need to setPromiscuous(true) to receive all frames
    rf95.setPromiscuous(true);

    // set delimeter
    if (mode == delimited) {
        if (DELIMETER_CHAR != '~') {
            for (int i = 0; i < sizeof(formatString); i++)
            {
                if (formatString[i] == '~')
                    formatString[i] = DELIMETER_CHAR;
            }
            for (int i = 0; i < sizeof(legendString); i++)
            {
                if (legendString[i] == '~')
                    legendString[i] = DELIMETER_CHAR;
            }
        }
        Serial.println(legendString);
    }
}


void loop()
{
    // wait for a lora packet
    rxRecvLen = sizeof(rxBuffer);               // RadioHead expects max buffer, will update to received bytes
    digitalWrite(LED, LOW);

    if (rf95.available())
    { 
        digitalWrite(LED, HIGH);
        if (rf95.recv(rxBuffer, &rxRecvLen))
        {
            char isAck[4] = {""};
            if (rf95.headerFlags() & RH_FLAGS_ACK)
                memcpy(isAck, "Ack\0", 3);
            rxBuffer[rxRecvLen] = '\0';
            
            if (mode == delimited)
            {
                snprintf(printBuffer, sizeof(printBuffer), formatString, rxCount++, millis(), rf95.lastRssi(), rf95.headerFrom(), rf95.headerTo(), rf95.headerId(), rf95.headerFlags(), isAck, rxRecvLen, rxBuffer);
                Serial.println(printBuffer);
            }
            else
            {
                snprintf(printBuffer, sizeof(printBuffer), "Recv#:%d @ %d,   Signal(RSSI)= %d", rxCount++, millis(), rf95.lastRssi());
                Serial.println(printBuffer);
                snprintf(printBuffer, sizeof(printBuffer), " From: %d >> To: %d     MsgId: %d  Flags: %2x    %s", rf95.headerFrom(), rf95.headerTo(), rf95.headerId(), rf95.headerFlags(), isAck);
                Serial.println(printBuffer);
                snprintf(printBuffer, sizeof(printBuffer), "Bytes: %d => %s \r\n", rxRecvLen, rxBuffer);
                Serial.println(printBuffer);
            }
        }
    }
}
    
