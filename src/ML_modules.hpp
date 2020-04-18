#include "ML_components.hpp"

#include "rack.hpp"

//#ifdef v_dev
//#define Davies1900hSmallBlackKnob SmallMLKnob
//#endif

using namespace rack;

extern Plugin *pluginInstance;

struct NKK2 : SvgSwitch { //Toggle
	NKK2() {
		addFrame(APP->window->loadSvg(asset::system("res/ComponentLibrary/NKK_0.svg")));
		addFrame(APP->window->loadSvg(asset::system("res/ComponentLibrary/NKK_2.svg")));
	}
};

struct POLSWITCH : SvgSwitch { //Toggle
	POLSWITCH() {
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CKSS_0.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CKSS_1.svg")));
	}
};


/*
struct MLKnob : RoundKnob {
	MLKnob() {
		setSVG(APP->window->loadSvg(asset::plugin(pluginInstance,"res/Knob.svg")));
	}
};

struct RedMLKnob : RoundKnob {
	RedMLKnob() {
		setSVG(APP->window->loadSvg(asset::plugin(pluginInstance,"res/RedKnob.svg")));
	}
};

struct SmallMLKnob : RoundKnob {
	SmallMLKnob() {
		setSVG(APP->window->loadSvg(asset::plugin(pluginInstance,"res/Knob_28.svg")));
	}
};

struct SmallRedMLKnob : RoundKnob {
	SmallRedMLKnob() {
		setSVG(APP->window->loadSvg(asset::plugin(pluginInstance,"res/RedKnob_28.svg")));
	}
};
*/


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
