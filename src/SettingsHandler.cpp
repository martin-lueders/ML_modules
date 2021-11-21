#include "SettingsHandler.hpp"

void SettingsHandler::save(json_t* rootJ) {

	std::string filename = asset::user("ML_Modules.json");
	
	FILE *file = fopen(filename.c_str(), "w");
	
	if (file) {
		json_dumpf(rootJ, file, JSON_INDENT(2) | JSON_REAL_PRECISION(9));
		fclose(file);
	}
};

json_t* SettingsHandler::read()  {

    json_t *rootJ;

	std::string filename = asset::user("ML_Modules.json");
	FILE *file = fopen(filename.c_str(), "r");
	
	if (!file) {
		return json_object();
	}
	
	json_error_t error;
	rootJ = json_loadf(file, 0, &error);
	
	fclose(file);
    return rootJ;
}


int  SettingsHandler::getGlobalInt(std::string key) { 

    int value = 0;

    json_t *rootJ = read();

	json_t* valueJ = json_object_get(rootJ, key.c_str());
	if (valueJ)
		value = json_integer_value(valueJ);

	json_decref(rootJ);

    return value;     
};

void SettingsHandler::setGlobalInt(std::string key, int value) {

    json_t *rootJ = read();

	json_object_set_new(rootJ, key.c_str(), json_integer(value));

    save(rootJ);
    json_decref(rootJ);
};



int  SettingsHandler::getInt(std::string key) { 

    int value = 0;

    json_t *rootJ = read();
    json_t *valueJ = nullptr;
    json_t *moduleJ = json_object_get(rootJ, moduleName.c_str());

    if (moduleJ) valueJ = json_object_get(moduleJ, key.c_str());
	if (valueJ)	 value  = json_integer_value(valueJ);

	json_decref(rootJ);

    return value;     
};

void SettingsHandler::setInt(std::string key, int value) {

    json_t *rootJ = read();
    json_t *moduleJ = json_object_get(rootJ, moduleName.c_str());

    if (moduleJ == nullptr) moduleJ = json_object();

	json_object_set_new(moduleJ, key.c_str(), json_integer(value));
	json_object_set_new(rootJ, moduleName.c_str(), moduleJ);

    save(rootJ);
    json_decref(rootJ);
};


bool SettingsHandler::getBool(std::string key) { 

    bool value = false;

    json_t *rootJ = read();
    json_t *valueJ = nullptr;
    json_t *moduleJ = json_object_get(rootJ, moduleName.c_str());

    if (moduleJ) valueJ = json_object_get(moduleJ, key.c_str());
	if (valueJ)	 value  = !!json_integer_value(valueJ);

	json_decref(rootJ);

    return value;     
};

void SettingsHandler::setBool(std::string key, bool value) {

    json_t *rootJ = read();
    json_t *moduleJ = json_object_get(rootJ, moduleName.c_str());

    if (moduleJ == nullptr) moduleJ = json_object();

	json_object_set_new(moduleJ, key.c_str(), json_integer(value));
	json_object_set_new(rootJ, moduleName.c_str(), moduleJ);

    save(rootJ);
    json_decref(rootJ);
};


