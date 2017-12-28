#include "ML_modules.hpp"
#include <math.h>

Plugin *plugin;



void init(rack::Plugin *p) {
 	plugin = p;
 	plugin->slug="ML_modules";
	plugin->version = TOSTRING(VERSION);


 	plugin->addModel(createModel<QuantizerWidget>("ML modules", "Quantizer", "Quantizer (h-bar)", QUANTIZER_TAG));
 	plugin->addModel(createModel<QuantumWidget>("ML modules", "Quantum", "Quantum", QUANTIZER_TAG));
 	plugin->addModel(createModel<TrigBufWidget>("ML modules", "TrigBuf", "Trigger Buffer", UTILITY_TAG));
	plugin->addModel(createModel<SeqSwitchWidget>("ML modules", "SeqSwitch", "Sequential Switch 8->1",SWITCH_TAG, SEQUENCER_TAG));
	plugin->addModel(createModel<SeqSwitch2Widget>("ML modules", "SeqSwitch2", "Sequential Switch 1->8", SWITCH_TAG));
	plugin->addModel(createModel<ShiftRegisterWidget>("ML modules", "ShiftRegister", "Shift Register",SAMPLE_AND_HOLD_TAG));
	plugin->addModel(createModel<ShiftRegister2Widget>("ML modules", "Evolution", "Evolution", SEQUENCER_TAG, SAMPLE_AND_HOLD_TAG));
	plugin->addModel(createModel<FreeVerbWidget>("ML modules", "FreeVerb", "FreeVerb", REVERB_TAG));
	plugin->addModel(createModel<Sum8Widget>("ML modules", "Sum8", "Sum8", UTILITY_TAG, MIXER_TAG));
	plugin->addModel(createModel<Sum8mk2Widget>("ML modules", "Sum8mk2", "Sum8 MkII", UTILITY_TAG, MIXER_TAG));
	plugin->addModel(createModel<SH8Widget>("ML modules", "SH8", "S&H 8", SAMPLE_AND_HOLD_TAG));
	plugin->addModel(createModel<ConstantsWidget>("ML modules", "Constants", "Constants", UTILITY_TAG));
	plugin->addModel(createModel<CounterWidget>("ML modules", "Counter", "Counter", UTILITY_TAG));
	plugin->addModel(createModel<TrigDelayWidget>("ML modules", "TrigDelay", "Trigger Delay", UTILITY_TAG, DELAY_TAG));
	plugin->addModel(createModel<BPMdetectWidget>("ML modules", "BPMdetect", "BPM Tools", UTILITY_TAG, CLOCK_TAG));
	plugin->addModel(createModel<VoltMeterWidget>("ML modules", "VoltMeter", "Volt Meter", UTILITY_TAG));
	plugin->addModel(createModel<OctaFlopWidget>("ML modules", "OctaFlop", "OctaFlop", UTILITY_TAG, CLOCK_TAG));
	plugin->addModel(createModel<OctaTrigWidget>("ML modules", "OctaTrig", "OctaTrig", UTILITY_TAG ));
	plugin->addModel(createModel<OctaSwitchWidget>("ML modules", "OctaSwitch", "OctaSwitch", SWITCH_TAG, UTILITY_TAG ));
	plugin->addModel(createModel<TrigSwitchWidget>("ML modules", "TrigSwitch", "TrigSwitch 8->1", SWITCH_TAG, UTILITY_TAG ));
 	plugin->addModel(createModel<TrigSwitch2Widget>("ML modules", "TrigSwitch2", "TrigSwitch 1->8", SWITCH_TAG, UTILITY_TAG ));
}




