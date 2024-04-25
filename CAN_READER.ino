#include <SPI.h>
#include "mcp_can.h"

// the cs pin of the version v1.4 is default to D10
const int SPI_CS_PIN = 10;

MCP_CAN CAN(SPI_CS_PIN); // Set CS pin

void setup() {
    Serial.begin(115200);

    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
    }

START_INIT:

    if (CAN_OK == CAN.begin(CAN_500KBPS)) { // init can bus: baudrate = 500k
        Serial.println("CAN BUS Shield init ok!");
    } else {
        Serial.println("CAN BUS Shield init fail");
        Serial.println("Init CAN BUS Shield again");
        delay(100);
        goto START_INIT;
    }
}

void loop() {
    unsigned char len = 0;
    unsigned char buf[16];

    // Check for Serial input to send CAN messages
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n'); // Read the command until newline character
        command.trim(); // Trim any whitespace

        if (command == "START") {
            unsigned char startMsg[8] = {0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00};
            unsigned char startMsg2[8] = {0x02, 0x03, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
            CAN.sendMsgBuf(0x560, 0, 8, startMsg); // Send CAN message
            CAN.sendMsgBuf(0x560, 0, 8, startMsg2); // Send CAN message
            Serial.println("Sent START message to CAN");
        } else if (command == "STOP") {
            unsigned char stopMsg[8] = {0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00};
            unsigned char stopMsg1[8] = {0x01, 0x05, 0x09, 0x04, 0x00, 0x00, 0x00, 0x00};
            CAN.sendMsgBuf(0x80000036, 1, 8, stopMsg); // Send CAN message
            CAN.sendMsgBuf(0x4FFFFF36, 1, 8, stopMsg1); // Send CAN message
            Serial.println("Sent STOP message to CAN");
        }
    }

    // Check for incoming CAN messages
    if (CAN_MSGAVAIL == CAN.checkReceive()) {
        CAN.readMsgBuf(&len, buf); // read data, len: data length, buf: data buf
        unsigned long canId = CAN.getCanId(); // Ensure canId is long to handle extended IDs
        bool isExtended = CAN.isExtendedFrame(); // Check if the frame is an extended CAN frame




        //*********************************************************************************
        //**************************.    Write CAN MSG      ********************************
        //*********************************************************************************

        //unsigned char startMsg[8] = {0x10, 0x13, 0x04, 0x01, 0x02, 0x03, 0x03, 0x03};
        //CAN.sendMsgBuf(0x1376, 0, 8, startMsg); // Send CAN message
        


        //*********************************************************************************
        //**************************.    READ CAN MSG      ********************************
        //*********************************************************************************
        if (canId) { // if you want to filter any ID you can set a filter like canID==0x000
            Serial.println("-----------------------------");
            Serial.print("Get data from ID: ");
            Serial.print(canId); // Decimal ID
            Serial.print(" (0x");
            Serial.print(canId, HEX); // Hexadecimal ID
            Serial.print(") ");
            Serial.print(isExtended ? "Extended ID" : "Standard ID"); // Show if ID is extended or standard
            Serial.println();

            Serial.print("Data received: ");
            for (int i = 0; i < len; i++) { // print the data
                Serial.print(buf[i]); // Decimal value of data
                Serial.print(" (0x");
                Serial.print(buf[i], HEX); // Hexadecimal value of data
                Serial.print(")\t");
            }
            Serial.println();
        }
    }
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
