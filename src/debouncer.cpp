#include "debouncer.h"

#define MAX_TEMP 30
#define MIN_TEMP 10
#define DEFAULT_TEMP 23

DebouncedEncoder::DebouncedEncoder(int clockPin, int dataPin) :
    encoder(clockPin, dataPin),
    reading(DEFAULT_TEMP),
    oldPosition(0),
    changed(false),
    lastChangeTime(millis()) {

}

void DebouncedEncoder::update() {
    int32_t newPosition = encoder.read();

    if (newPosition != oldPosition) {
        char report[256];
        sprintf(report, "Check: old: %d, new: %d, now: %ld, prev: %ld", oldPosition, newPosition, millis(), lastChangeTime);
        lastChangeTime = millis();
        Serial.println(report);
        changed = true;
        if ((newPosition - oldPosition) > 1 && reading < MAX_TEMP) {
            Serial.println("Incremented");
            ++reading;
        } else if ((newPosition - oldPosition) < -1 && reading > MIN_TEMP) {
            Serial.println("Decremented");
            --reading;
        }
    } else {
        changed = false;
    }
    oldPosition = newPosition;
}

int DebouncedEncoder::value() {
    return reading;
}

bool DebouncedEncoder::hasChanged() {
    return changed;
}

long DebouncedEncoder::lastChanged() {
    return lastChangeTime;
}