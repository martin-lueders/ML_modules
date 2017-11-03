#include "ML_modules.hpp"
#include <math.h>

Plugin *plugin;



void init(rack::Plugin *p) {
 	plugin = p;
 	plugin->slug="ML_modules";


#ifdef v040	
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
#endif

#ifdef v_dev
 	plugin->addModel(createModel<QuantizerWidget>("ML_modules", "ML modules", "Quantizer", "Quantizer (h-bar)"));
 	plugin->addModel(createModel<QuantumWidget>("ML_modules", "ML modules", "Quantum", "Quantum"));
 	plugin->addModel(createModel<TrigBufWidget>("ML_modules", "ML modules", "TrigBuf", "Trigger Buffer"));
	plugin->addModel(createModel<SeqSwitchWidget>("ML_modules", "ML modules", "SeqSwitch", "Sequential Switch 8->1"));
	plugin->addModel(createModel<SeqSwitch2Widget>("ML_modules", "ML modules", "SeqSwitch2", "Sequential Switch 1->8"));
	plugin->addModel(createModel<ShiftRegisterWidget>("ML_modules", "ML modules", "ShiftRegister", "Shift Register"));
	plugin->addModel(createModel<ShiftRegister2Widget>("ML_modules", "ML modules", "ShiftRegister2", "Shift Register 2"));
	plugin->addModel(createModel<FreeVerbWidget>("ML_modules", "ML modules", "FreeVerb", "FreeVerb"));
	plugin->addModel(createModel<Sum8Widget>("ML_modules", "ML modules", "Sum8", "Sum8"));
	plugin->addModel(createModel<SH8Widget>("ML_modules", "ML modules", "SH8", "S&H 8"));
	plugin->addModel(createModel<ConstantsWidget>("ML_modules", "ML modules", "Constants", "Constants"));
	plugin->addModel(createModel<CounterWidget>("ML_modules", "ML modules", "Counter", "Counter"));
	plugin->addModel(createModel<TrigDelayWidget>("ML_modules", "ML modules", "TrigDelay", "Trigger Delay"));
	plugin->addModel(createModel<BPMdetectWidget>("ML_modules", "ML modules", "BPMdetect", "BPM Detect"));
	plugin->addModel(createModel<VoltMeterWidget>("ML_modules", "ML modules", "VoltMeter", "Volt Meter"));
	plugin->addModel(createModel<OctaFlopWidget>("ML_modules", "ML modules", "OctaFlop", "OctaFlop"));
#endif
 }




