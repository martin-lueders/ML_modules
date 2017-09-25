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
	json_t *toJsonData();
	void fromJsonData(json_t *root);
};

struct TrigBufWidget : ModuleWidget {
	TrigBufWidget();
};

struct SeqSwitchWidget : ModuleWidget {
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
