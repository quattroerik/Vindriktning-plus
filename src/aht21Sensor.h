#include <Adafruit_AHTX0.h>

namespace ahtRead {

    Adafruit_AHTX0 aht;

    void setup() {
        aht.begin();
    }

    void parseAHT(ahtSensorState& state) {
        sensors_event_t humidity, temp;
        aht.getEvent(&humidity, &temp);
        state.humidity = humidity.relative_humidity;
        state.temperature = temp.temperature;

    }
}