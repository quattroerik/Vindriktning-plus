struct particleSensorState_t {
    uint16_t avgPM25 = 0;
    uint16_t measurements[5] = {0, 0, 0, 0, 0};
    uint8_t measurementIdx = 0;
    boolean valid = false;
};

struct bh1750SensorState {
    float lux = 0;
    float lastState = 0;
};

struct ahtSensorState {
    float temperature = 0;
    float humidity = 0;
};

struct pirSensorState{
    u_int8_t enabled = 0;
    u_int8_t lastState = 0;
    uint32_t timestamp = 0;
    
};
