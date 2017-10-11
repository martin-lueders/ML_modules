#include "ML_modules.hpp"
#include <math.h>
#include "dsp.hpp"


Plugin *plugin;

#ifdef v032
struct ML_modulesPlugin : Plugin {
	ML_modulesPlugin() {
		slug = "ML_Modules";
		name = "ML Modules";
		createModel<QuantizerWidget>(this, "Quantizer", "Quantizer (h-bar)");
		createModel<QuantumWidget>(this, "Quantum", "Quantum");
		createModel<TrigBufWidget>(this, "TrigBuf", "Trigger Buffer");
		createModel<SeqSwitchWidget>(this, "SeqSwitch", "Sequential Switch");
		createModel<ShiftRegisterWidget>(this, "ShiftRegister", "Shift Register");
		createModel<FreeVerbWidget>(this, "FreeVerb", "FreeVerb");
		createModel<Sum8Widget>(this, "Sum8", "Sum8");
	}
};

Plugin* init() {
	return new ML_modulesPlugin();
};
#endif

#ifdef v040

void init(rack::Plugin *p) {
 	plugin = p;
 	plugin->slug="ML_modules";
 	plugin->name="ML modules";
 	plugin->homepageUrl="https://github.com/martin-lueders/ML_modules";
	
 	createModel<QuantizerWidget>(plugin, "Quantizer", "Quantizer (h-bar)");
 	createModel<QuantumWidget>(plugin, "Quantum", "Quantum");
 	createModel<TrigBufWidget>(plugin, "TrigBuf", "Trigger Buffer");
	createModel<SeqSwitchWidget>(plugin, "SeqSwitch", "Sequential Switch");
	createModel<ShiftRegisterWidget>(plugin, "ShiftRegister", "Shift Register");
	createModel<FreeVerbWidget>(plugin, "FreeVerb", "FreeVerb");
	createModel<Sum8Widget>(plugin, "Sum8", "Sum8");

 }

#endif



