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
	createModel<ShiftRegister2Widget>(plugin, "Evolution", "Evolution");
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
 	plugin->addModel(createModel<QuantizerWidget>("ML modules", "Quantizer", "Quantizer (h-bar)", QUANTIZER_TAG));
 	plugin->addModel(createModel<QuantumWidget>("ML modules", "Quantum", "Quantum", QUANTIZER_TAG));
 	plugin->addModel(createModel<TrigBufWidget>("ML modules", "TrigBuf", "Trigger Buffer", UTILITY_TAG));
	plugin->addModel(createModel<SeqSwitchWidget>("ML modules", "SeqSwitch", "Sequential Switch 8->1",SWITCH_TAG, SEQUENCER_TAG));
	plugin->addModel(createModel<SeqSwitch2Widget>("ML modules", "SeqSwitch2", "Sequential Switch 1->8", SWITCH_TAG));
	plugin->addModel(createModel<ShiftRegisterWidget>("ML modules", "ShiftRegister", "Shift Register",SAMPLE_AND_HOLD_TAG));
	plugin->addModel(createModel<ShiftRegister2Widget>("ML modules", "Evolution", "Evolution", SEQUENCER_TAG, SAMPLE_AND_HOLD_TAG));
	plugin->addModel(createModel<FreeVerbWidget>("ML modules", "FreeVerb", "FreeVerb", REVERB_TAG));
	plugin->addModel(createModel<Sum8Widget>("ML modules", "Sum8", "Sum8", UTILITY_TAG, MIXER_TAG));
	plugin->addModel(createModel<SH8Widget>("ML modules", "SH8", "S&H 8", SAMPLE_AND_HOLD_TAG));
	plugin->addModel(createModel<ConstantsWidget>("ML modules", "Constants", "Constants", UTILITY_TAG));
	plugin->addModel(createModel<CounterWidget>("ML modules", "Counter", "Counter", UTILITY_TAG));
	plugin->addModel(createModel<TrigDelayWidget>("ML modules", "TrigDelay", "Trigger Delay", UTILITY_TAG, DELAY_TAG));
	plugin->addModel(createModel<BPMdetectWidget>("ML modules", "BPMdetect", "BPM Tools", UTILITY_TAG, CLOCK_TAG));
	plugin->addModel(createModel<VoltMeterWidget>("ML modules", "VoltMeter", "Volt Meter", UTILITY_TAG));
	plugin->addModel(createModel<OctaFlopWidget>("ML modules", "OctaFlop", "OctaFlop", UTILITY_TAG, CLOCK_TAG));
	plugin->addModel(createModel<OctaTrigWidget>("ML modules", "OctaTrig", "OctaTrig", UTILITY_TAG ));
#endif
 }




