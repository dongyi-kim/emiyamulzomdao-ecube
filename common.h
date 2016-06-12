#include <iostream>
#include <pthread.h>
using namespace std;

#ifndef SYSPROG_COMMON_H
#define SYSPROG_COMMON_H

#define NOT_AUTHORIZED -1
#define OBSERVE_MODE 1
#define EDIT_MODE 2

class Data {
public:
    int illumination, temperature, humidity, soil_humidity;
    Data(){
        illumination = 0;
        temperature = 0;
        humidity = 0;
        soil_humidity = 0;
    }
    Data(int illumination, int temperature, int humidity, int soil_humidity) {
        this->illumination = illumination;
        this->temperature = temperature;
        this->humidity = humidity;
        this->soil_humidity = soil_humidity;
    }
};

class State {
public:
    int state[10];
    int len;

    State() {
        this->len = 0;
    }
};

class Shared {
public:
    Data data;
    Data sensor;
    State state;
    int mode;
    std::string id;
    bool liq_exist;
    int segValue;
    Shared(){
        liq_exist = true;
        segValue = -1;
        this->mode = -1;
    }
};


namespace edit{
    extern void init();
    extern void* edit(void* shared);
};

namespace observe{
    extern void init();
    extern void* observe(void* shared);
};

namespace auth{
    void input_account(string &id);
    int authorize(string id, string* result);
}

#endif //SYSPROG_COMMON_H