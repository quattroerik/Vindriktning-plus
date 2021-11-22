struct particleSensorState_t {
    uint16_t avgPM25;
    uint16_t measurements[5] = {0, 0, 0, 0, 0};
    uint8_t measurementIdx = 0;
};


struct bh1750SensorState {
    float lux;
    float lastState;
};

struct ahtSensorState {
    float temperature;
    float humidity;
};

struct irSensorState{
    u_int8_t enabled;
    u_int8_t lastState;
    uint32_t timestamp;
    
};

struct mqttDataSet {
    String time;
    String date;
    String tempOut;
    String rhOut;
    String bDay;
    String pwd;
    String soundTouch;
    u_int8_t dimState;
    u_int8_t soundTouchPreset;
    u_int8_t airFilter;
    u_int8_t nightMode;
    u_int8_t armed;
};
