#include <iostream>
using namespace std;

#ifndef SYSPROG_COMMON_H
#define SYSPROG_COMMON_H

#define NOT_AUTHORIZED -1
#define OBSERVE_MODE 1
#define EDIT_MODE 2

class Data {
public:
    int illumination, humidity, temperature, soil_humidity;
    Data(){}
    Data(int illumination, int temperature, int humidity, int soil_humidity) {
        this->illumination = illumination;
        this->temperature = temperature;
        this->humidity = humidity;
        this->soil_humidity = soil_humidity;
    }
};

class Shared {
public:
    Data data;
    int mode;
    std::string id;
    Shared(){
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