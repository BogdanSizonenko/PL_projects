#include <Arduino.h>

#define UART_RX_PIN 2 // GPIO2
#define UART_TX_PIN 4 // GPIO4
#define BINDBUTTON 35 // GRPIO35

const int bindButton = BINDBUTTON;
int timeToPushButton = 2000;
const char bindFrame[] = {
            0xC8,
            0x07,
            0x32,
            0xEC,
            0xC8,
            0x10,
            0x01,
            0x9E,
            0xE8,
        };

void setup(){
    Serial.begin(9600);
    Serial2.begin(416666, SERIAL_8N1,
                  UART_RX_PIN, 
                  UART_TX_PIN);
    pinMode(bindButton, INPUT_PULLUP);
    pinMode(34, INPUT);
}

void loop(){
    if (digitalRead(bindButton) == LOW){
        delay(500);
        int currentTime = millis();
        while((millis() - currentTime) < timeToPushButton){
        }
        Serial.println("Bind command sended!");
        Serial2.write(bindFrame);
        delay(10000);
    }
}