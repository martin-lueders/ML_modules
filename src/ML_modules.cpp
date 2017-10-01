#include "ML_modules.hpp"
#include <math.h>
#include "dsp.hpp"


// Plugin *plugin;


struct ML_modulesPlugin : Plugin {
	ML_modulesPlugin() {
		slug = "ML_Modules";
		name = "ML Modules";
		createModel<QuantizerWidget>(this, "Quantizer", "Quantizer (h-bar)");
		createModel<QuantumWidget>(this, "Quantum", "Quantum");
		createModel<TrigBufWidget>(this, "TrigBuf", "Trigger Buffer");
		createModel<SeqSwitchWidget>(this, "SeqSwitch", "Sequential Switch");
	}
};



// void init(rack::Plugin *p) {
// 	plugin = p;
// 	plugin->slug="ML_modules";
// 	plugin->name="ML modules";
// 	plugin->homepageURL="https://github.com/martin-lueders/ML-modules";
	
// 	createModel<QuantizerWidget>(plugin, "Quantizer", "Quantizer (h-bar)");
// 	createModel<QuantumWidget>(plugin, "Quantum", "Quantum");
// 	createModel<TrigBufWidget>(plugin, "TrigBuf", "Trigger Buffer");
// }


Plugin* init() {
	return new ML_modulesPlugin();
};

