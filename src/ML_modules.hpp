#include "rack.hpp"

//#ifdef v_dev
//#define Davies1900hSmallBlackKnob SmallMLKnob
//#endif

using namespace rack;

extern Plugin *plugin;


struct MLKnob : RoundKnob {
	MLKnob() {
		setSVG(SVG::load(assetPlugin(plugin,"res/Knob.svg")));
		box.size = Vec(36, 36);
	}
};

struct RedMLKnob : RoundKnob {
	RedMLKnob() {
		setSVG(SVG::load(assetPlugin(plugin,"res/RedKnob.svg")));
		box.size = Vec(36, 36);
	}
};

struct SmallMLKnob : MLKnob {
	SmallMLKnob() {
		box.size = Vec(28, 28);
	}
};

struct SmallRedMLKnob : RedMLKnob {
	SmallRedMLKnob() {
		box.size = Vec(28, 28);
	}
};

////////////////////
// module widgets
////////////////////

struct QuantizerWidget : ModuleWidget {
	QuantizerWidget();
};

struct QuantumWidget : ModuleWidget {
	QuantumWidget();
	json_t *toJsonData() ;
	void fromJsonData(json_t *root) ;
	Menu *createContextMenu() override;
};

struct TrigBufWidget : ModuleWidget {
	TrigBufWidget();
};

struct SeqSwitchWidget : ModuleWidget {
	SeqSwitchWidget();
	json_t *toJsonData() ;
	void fromJsonData(json_t *root) ;
	Menu *createContextMenu() override;
};

struct SeqSwitch2Widget : ModuleWidget {
	SeqSwitch2Widget();
	json_t *toJsonData() ;
	void fromJsonData(json_t *root) ;
	Menu *createContextMenu() override;
};

struct ShiftRegisterWidget : ModuleWidget {
	ShiftRegisterWidget();
};

struct ShiftRegister2Widget : ModuleWidget {
	ShiftRegister2Widget();
};

struct FreeVerbWidget : ModuleWidget {
	FreeVerbWidget();
};

struct Sum8Widget : ModuleWidget {
	Sum8Widget();
};

struct SH8Widget : ModuleWidget {
	SH8Widget();
};

struct ConstantsWidget : ModuleWidget {
	ConstantsWidget();
};

struct CounterWidget : ModuleWidget {
	CounterWidget();
};

struct TrigDelayWidget : ModuleWidget {
	TrigDelayWidget();
};

struct BPMdetectWidget : ModuleWidget {
	BPMdetectWidget();
};

struct VoltMeterWidget : ModuleWidget {
	VoltMeterWidget();
	TextField ** label;
};

struct OctaFlopWidget : ModuleWidget {
	OctaFlopWidget();
};
