#include <Encoder.h>

class DebouncedEncoder {
    private:
        Encoder encoder;
        int32_t reading;
        int32_t oldPosition;
        bool changed;
        long lastChangeTime;

    public:
        DebouncedEncoder(int clockPin, int dataPin);
        void update();
        int value();
        bool hasChanged();
        long lastChanged();
};