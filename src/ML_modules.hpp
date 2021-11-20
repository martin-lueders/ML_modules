#include "ML_components.hpp"

#include "rack.hpp"


#define MAX(a,b) a>b?a:b
#define MIN(a,b) a>b?b:a

using namespace rack;

extern Plugin *pluginInstance;





extern Model *modelQuantizer;
extern Model *modelQuantum;
extern Model *modelTrigBuf;
extern Model *modelSeqSwitch;
extern Model *modelSeqSwitch2;
extern Model *modelShiftRegister;
extern Model *modelShiftRegister2;
extern Model *modelFreeVerb;
extern Model *modelSum8;
extern Model *modelSum8mk2;
extern Model *modelSH8;
extern Model *modelConstants;
extern Model *modelCounter;
extern Model *modelTrigDelay;
extern Model *modelBPMdetect;
extern Model *modelVoltMeter;
extern Model *modelOctaFlop;
extern Model *modelOctaTrig;
extern Model *modelOctaSwitch;
extern Model *modelTrigSwitch;
extern Model *modelTrigSwitch2;
extern Model *modelTrigSwitch3;
extern Model *modelTrigSwitch3_2;
extern Model *modelOctaPlus;
extern Model *modelOctaTimes;
extern Model *modelCloner;
extern Model *modelPolySplitter;
extern Model *modelArpeggiator;
