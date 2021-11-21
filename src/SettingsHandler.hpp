#pragma once

#include "rack.hpp"
#include <string>

using namespace rack;

class SettingsHandler {

    std::string moduleName;

    //SettingsHandler() {};
    //~SettingsHandler() {};

public:

    void setModule(std::string name) {moduleName = name;};

    void save(json_t*);
    json_t* read();

    int  getInt(std::string);
    void setInt(std::string, int);

    int  getGlobalInt(std::string);
    void setGlobalInt(std::string, int);

    bool getBool(std::string);
    void setBool(std::string, bool);

    bool getGlobalBool(std::string);
    void setGlobalBool(std::string, bool);

};
