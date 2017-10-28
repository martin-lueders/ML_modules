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

struct FreeVerbWidget : ModuleWidget {
	FreeVerbWidget();
};

struct Sum8Widget : ModuleWidget {
	Sum8Widget();
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
};
