
#define IRPIN 14  //D5

namespace IRread {
    void setup() {
        pinMode(IRPIN,INPUT);
    }

    void parseIR(irSensorState& state) {
        u_int8_t tmp = digitalRead(IRPIN);
        if(tmp == 1) {
            state.timestamp = millis();
        }
        state.lastState = state.enabled;
        state.enabled = tmp;
    }
}