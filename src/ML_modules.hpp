#include "rack.hpp"


using namespace rack;

#ifdef v040
extern Plugin *plugin;
#endif

////////////////////
// module widgets
////////////////////

struct QuantizerWidget : ModuleWidget {
	QuantizerWidget();
};

struct QuantumWidget : ModuleWidget {
	QuantumWidget();
	json_t *toJsonData();
	void fromJsonData(json_t *root);
};

struct TrigBufWidget : ModuleWidget {
	TrigBufWidget();
};

struct SeqSwitchWidget : ModuleWidget {
	SeqSwitchWidget();
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
