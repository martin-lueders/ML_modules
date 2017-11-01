#include "ML_modules.hpp"
#include <math.h>

Plugin *plugin;



void init(rack::Plugin *p) {
 	plugin = p;
 	plugin->slug="ML_modules";
 	plugin->name="ML modules";
 	plugin->homepageUrl="https://github.com/martin-lueders/ML_modules";
	
 	createModel<QuantizerWidget>(plugin, "Quantizer", "Quantizer (h-bar)");
 	createModel<QuantumWidget>(plugin, "Quantum", "Quantum");
 	createModel<TrigBufWidget>(plugin, "TrigBuf", "Trigger Buffer");
	createModel<SeqSwitchWidget>(plugin, "SeqSwitch", "Sequential Switch 8->1");
	createModel<SeqSwitch2Widget>(plugin, "SeqSwitch2", "Sequential Switch 1->8");
	createModel<ShiftRegisterWidget>(plugin, "ShiftRegister", "Shift Register");
	createModel<ShiftRegister2Widget>(plugin, "ShiftRegister2", "Shift Register 2");
	createModel<FreeVerbWidget>(plugin, "FreeVerb", "FreeVerb");
	createModel<Sum8Widget>(plugin, "Sum8", "Sum8");
	createModel<SH8Widget>(plugin, "SH8", "S&H 8");
	createModel<ConstantsWidget>(plugin, "Constants", "Constants");
	createModel<CounterWidget>(plugin, "Counter", "Counter");
	createModel<TrigDelayWidget>(plugin, "TrigDelay", "Trigger Delay");
	createModel<BPMdetectWidget>(plugin, "BPMdetect", "BPM Detect");
	createModel<VoltMeterWidget>(plugin, "VoltMeter", "Volt Meter");
	createModel<OctaFlopWidget>(plugin, "OctaFlop", "OctaFlop");

 }




