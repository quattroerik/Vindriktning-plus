#define PIRPIN 14  //D5

namespace PIRread {
    void setup() {
        pinMode(PIRPIN,INPUT);
    }

    void parsePIR(pirSensorState& state) {
        u_int8_t tmp = digitalRead(PIRPIN);
        if(tmp == 1) {
            state.timestamp = millis();
        }
        state.lastState = state.enabled;
        state.enabled = tmp;
    }
}