#include "ML_components.hpp"

#include "rack.hpp"

//#ifdef v_dev
//#define Davies1900hSmallBlackKnob SmallMLKnob
//#endif

using namespace rack;

extern Plugin *plugin;

struct NKK2 : SVGSwitch, ToggleSwitch {
	NKK2() {
		addFrame(SVG::load(assetGlobal("res/ComponentLibrary/NKK_0.svg")));
		addFrame(SVG::load(assetGlobal("res/ComponentLibrary/NKK_2.svg")));
	}
};

struct POLSWITCH : SVGSwitch, ToggleSwitch {
	POLSWITCH() {
		addFrame(SVG::load(assetPlugin(plugin, "res/CKSS_0.svg")));
		addFrame(SVG::load(assetPlugin(plugin, "res/CKSS_1.svg")));
	}
};


/*
struct MLKnob : RoundKnob {
	MLKnob() {
		setSVG(SVG::load(assetPlugin(plugin,"res/Knob.svg")));
	}
};

struct RedMLKnob : RoundKnob {
	RedMLKnob() {
		setSVG(SVG::load(assetPlugin(plugin,"res/RedKnob.svg")));
	}
};

struct SmallMLKnob : RoundKnob {
	SmallMLKnob() {
		setSVG(SVG::load(assetPlugin(plugin,"res/Knob_28.svg")));
	}
};

struct SmallRedMLKnob : RoundKnob {
	SmallRedMLKnob() {
		setSVG(SVG::load(assetPlugin(plugin,"res/RedKnob_28.svg")));
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
