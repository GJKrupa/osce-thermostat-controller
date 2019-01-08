#include <Encoder.h>
#include <U8x8lib.h>

#include <Lora.h>

#define SS      8
#define RST     4
#define DI0     7
#define BAND    8681E5

#define MAX_TEMP 30
#define MIN_TEMP 10

Encoder myEnc(2, 3);
U8X8_SSD1306_128X32_UNIVISION_SW_I2C u8x8(11, 12, U8X8_PIN_NONE);

long oldPosition  = 0;
int temperature_x_2 = MIN_TEMP * 2;
char display[64];

void setup() {
  Serial.begin(115200);
  u8x8.begin();
  u8x8.setFont(u8x8_font_amstrad_cpc_extended_f);

  u8x8.drawString(0, 0, "Starting...");
  u8x8.refreshDisplay();

  LoRa.setPins(SS, RST, DI0);
  LoRa.enableCrc();
  LoRa.setSpreadingFactor(7);
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    u8x8.drawString(0, 1, "LoRa BAD!");
    u8x8.refreshDisplay();
    while (1);
  } else {
    u8x8.clearDisplay();
    u8x8.setFont(u8x8_font_px437wyse700a_2x2_f);
    sprintf(display, "%02d", temperature_x_2/2);
    u8x8.drawString(0, 0, display);
    u8x8.refreshDisplay();
  }
}

unsigned long lastChanged = 0;
bool sent = false;

void loop() {
  long newPosition = myEnc.read();
  if (newPosition != oldPosition) {
    if (newPosition > oldPosition && temperature_x_2 < MAX_TEMP*2) {
      ++temperature_x_2;
    } else if (newPosition < oldPosition && temperature_x_2 > MIN_TEMP*2) {
      --temperature_x_2;
    }
    sprintf(display, "%02d", temperature_x_2/2);
    if (sent) {
      u8x8.drawString(0, 3, "...");
      u8x8.setFont(u8x8_font_px437wyse700a_2x2_f);
    }
    u8x8.drawString(0, 0, display);
    u8x8.refreshDisplay();

    oldPosition = newPosition;
    lastChanged = millis();
    sent = false;
  } else if (millis() - lastChanged > 1000L && !sent) {
    u8x8.setFont(u8x8_font_amstrad_cpc_extended_f);
    u8x8.drawString(0, 3, "OK "); // Tick

    LoRa.beginPacket();
    LoRa.print(display);
    LoRa.endPacket();

    sent = true;
  }
}