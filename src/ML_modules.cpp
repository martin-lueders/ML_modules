#include "ML_modules.hpp"
#include <math.h>

Plugin *pluginInstance;



void init(rack::Plugin *p) {
 	pluginInstance = p;


 	pluginInstance->addModel(modelQuantizer);
 	pluginInstance->addModel(modelQuantum);
 	pluginInstance->addModel(modelTrigBuf);
	pluginInstance->addModel(modelSeqSwitch);
	pluginInstance->addModel(modelSeqSwitch2);
	pluginInstance->addModel(modelShiftRegister);
	pluginInstance->addModel(modelShiftRegister2);
	pluginInstance->addModel(modelFreeVerb);
	pluginInstance->addModel(modelSum8);
	pluginInstance->addModel(modelSum8mk2);
	pluginInstance->addModel(modelSH8);
	pluginInstance->addModel(modelConstants);
	pluginInstance->addModel(modelCounter);
	pluginInstance->addModel(modelTrigDelay);
	pluginInstance->addModel(modelBPMdetect);
	pluginInstance->addModel(modelVoltMeter);
	pluginInstance->addModel(modelOctaFlop);
	pluginInstance->addModel(modelOctaTrig);
	pluginInstance->addModel(modelOctaSwitch);
	pluginInstance->addModel(modelTrigSwitch);
 	pluginInstance->addModel(modelTrigSwitch2);
 	pluginInstance->addModel(modelTrigSwitch3);
 	pluginInstance->addModel(modelTrigSwitch3_2);
	pluginInstance->addModel(modelOctaPlus);
	pluginInstance->addModel(modelOctaTimes);
}






