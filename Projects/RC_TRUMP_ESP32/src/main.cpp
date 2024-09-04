#include <Arduino.h>
#include "hal/uart_types.h"

#define IRC_TRAMP_UART_BAUD 9600
#define VTX_RX_PIN 16
#define VTX_TX_PIN 5
#define CHANNEL_LED 34
#define CHANGE_CHANNEL_BUTTON 35


static uint8_t trampCrc(const uint8_t* frame)//const trampFrame_t* frame)
{
  uint8_t cksum = 0;
  for (int i = 1; i < 16 - 2; i++) 
  {
    cksum += frame[i];
  }

  return cksum;
}

const uint16_t freqs[16] = {
    3320, 3345, 3370, 3395, 3420, 3445, 3470, 3495, //A band
    3310, 3330, 3355, 3380, 3405, 3430, 3455, 3480, //B band
};

static int channel;
const int changeChannelButton = CHANGE_CHANNEL_BUTTON;
const int channelLed = CHANNEL_LED;
static int timeToPushButton = 500;

static const char* getFrame(int frequence){
    uint8_t buf[16];
    memset(buf, 0, 16);
    buf[0] = 0x0F; //TRAMP_SYNC_START
    buf[1] = 0x45; //TRAMP_COMMAND_SET_FREQ
    buf[2] = frequence & 0xff;
    buf[3] = (frequence >> 8) & 0xff ;
    buf[14] = trampCrc(buf);
    buf[15] = 0x00; //TRAMP_SYNC_STOP
    static const char nextChannelFrame[] = {
        0x0F, // TRAMP_SYNC_START
        buf[1],//0x45, // TRAMP_COMMAND_SET_FREQ
        buf[2],
        buf[3],
        buf[4],
        buf[5],
        buf[6],
        buf[7],
        buf[8],
        buf[9],
        buf[10],
        buf[11],
        buf[12],
        buf[13],
        buf[14], // CRC
        0x00, // TRAMP_SYNC_STOP
    };
    return nextChannelFrame;
}

void setup() {
    Serial.begin(9600);
    Serial2.begin(IRC_TRAMP_UART_BAUD, SERIAL_8N1,
                VTX_RX_PIN,
                VTX_TX_PIN);
    Serial2.setPins(-1, -1, -1, 4);
    Serial2.setMode(UART_MODE_RS485_HALF_DUPLEX);
    pinMode(changeChannelButton, INPUT_PULLUP);
    channel = 0;
}

void loop() {
    if (digitalRead(changeChannelButton) == LOW){
        delay(500);
        int currentTime = millis();
        while((millis() - currentTime) < timeToPushButton){
        }
        if ((channel + 1) < 16){
            channel += 1;
            Serial2.write(getFrame(freqs[channel]));
            Serial.print(freqs[channel]);
        } else {
            channel = 0;
            Serial2.write(getFrame(freqs[channel]));
            Serial.print(freqs[channel]);
        }
        delay(1000);
    }
}