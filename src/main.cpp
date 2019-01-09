#include "debouncer.h"
#include <U8x8lib.h>

#include <Lora.h>

#define SS      8
#define RST     4
#define DI0     7
#define BAND    8681E5

bool sent = true;

DebouncedEncoder encoder(2, 3);
U8X8_SSD1306_128X32_UNIVISION_SW_I2C u8x8(11, 12, U8X8_PIN_NONE);

void drawTemperature(int temperature) {
    char display[20];
    u8x8.setFont(u8x8_font_px437wyse700a_2x2_f);
    sprintf(display, "%02d", temperature);
    u8x8.drawString(0, 0, display);
}

void drawStatus() {
  u8x8.setFont(u8x8_font_amstrad_cpc_extended_f);
  if (sent) {
    u8x8.drawString(0, 3, "OK ");
  } else {
    u8x8.drawString(0, 3, "...");
  }
}

void sendTemperature(int temperature) {
    char message[20];
    sprintf(message, "SETTEMP,%02d", temperature);
    LoRa.beginPacket();
    LoRa.print(message);
    LoRa.endPacket();
}

void setup() {
  Serial.begin(115200);
  u8x8.begin();
  u8x8.clearDisplay();
  u8x8.setFont(u8x8_font_amstrad_cpc_extended_f);
  u8x8.drawString(0, 0, "Starting...");
  u8x8.refreshDisplay();

  LoRa.setPins(SS, RST, DI0);

  if (!LoRa.begin(BAND)) {
    u8x8.drawString(0, 1, "LoRa BAD!");
    u8x8.refreshDisplay();
    while (1);
  } else {
    u8x8.clearDisplay();
    drawTemperature(encoder.value());
    u8x8.refreshDisplay();
  }
}

void loop() {
  encoder.update();
  if (encoder.hasChanged()) {
    sent = false;
    drawTemperature(encoder.value());
    drawStatus();
    u8x8.refreshDisplay();
  } else if (millis() - encoder.lastChanged() > 1000L && !sent) {
    sendTemperature(encoder.value());
    sent = true;
    drawStatus();
    u8x8.refreshDisplay();
  }
}