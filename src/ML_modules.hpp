#include "rack.hpp"


using namespace rack;

extern Plugin *plugin;

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
