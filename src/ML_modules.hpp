#include "rack.hpp"


using namespace rack;


////////////////////
// module widgets
////////////////////

struct QuantizerWidget : ModuleWidget {
	QuantizerWidget();
};

struct QuantumWidget : ModuleWidget {
	QuantumWidget();
};

struct TrigBufWidget : ModuleWidget {
	TrigBufWidget();
};

struct SeqSwictWidget : ModuleWidget {
	SeqSwitchWidget();
};

// struct MultWidget : ModuleWidget {
// 	MultWidget();
// };


//struct SEQGWidget : ModuleWidget {
//	SEQGWidget();
//	json_t *toJsonData();
//	void fromJsonData(json_t *root);
//};
