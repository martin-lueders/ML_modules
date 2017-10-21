#include "ML_modules.hpp"
#include "dsp/digital.hpp"


struct Counter : Module {
	enum ParamIds {
		MAX_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		GATE_INPUT,
		START_INPUT,
		STOP_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		GATE_OUTPUT,
		START_OUTPUT,
		STOP_OUTPUT,
		NUM_OUTPUTS
	};

	Counter() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS) {};

	void step() override;

#ifdef v040
	void initialize() override {counter=0; state=false; state2=false;};
#endif
#ifdef v_dec
	void reset() override {counter=0; state=false; state2=false;};
#endif

	int counter = 0;
	bool state = false;
	bool state2 = false;
	float stateLight;

	SchmittTrigger startTrigger, gateTrigger, stopTrigger;
	PulseGenerator startPulse, stopPulse;
};



void Counter::step() {


	if( startTrigger.process(inputs[START_INPUT].normalize(0.0) )) {
		state=true; 
		counter=0;
		startPulse.trigger(0.001);
	};

	if( stopTrigger.process(inputs[STOP_INPUT].normalize(0.0) ))   {
		state=false; 
		counter=0;
	};

	if( gateTrigger.process(inputs[GATE_INPUT].normalize(0.0) ) ) {

		if(state) counter++; 

		if(counter > params[MAX_PARAM].value) {
			
			counter = 0;
			state   = false;
			stopPulse.trigger(0.001);
		};
		
	};


	float out = (gateTrigger.isHigh()&&state) ? 10.0 : 0.0;

	float start = (startPulse.process(1.0/gSampleRate)) ? 10.0 : 0.0;
	float stop  = (stopPulse.process(1.0/gSampleRate)) ? 10.0 : 0.0;

	outputs[GATE_OUTPUT].value = out;
	outputs[START_OUTPUT].value = start;
	outputs[STOP_OUTPUT].value = stop;

}


CounterWidget::CounterWidget() {
	Counter *module = new Counter();
	setModule(module);
	box.size = Vec(15*3, 380);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin,"res/Counter.svg")));
		addChild(panel);
	}

	addChild(createScrew<ScrewSilver>(Vec(15, 0)));
	addChild(createScrew<ScrewSilver>(Vec(15, 365)));

	addParam(createParam<Davies1900hSmallBlackKnob>(Vec(9,  60), module, Counter::MAX_PARAM, 0.0, 128.0, 8.0));

	addInput(createInput<PJ301MPort>(Vec(10, 100), module, Counter::GATE_INPUT));
	addInput(createInput<PJ301MPort>(Vec(10, 135), module, Counter::START_INPUT));
	addInput(createInput<PJ301MPort>(Vec(10, 170), module, Counter::STOP_INPUT));

	addOutput(createOutput<PJ301MPort>(Vec(10, 200), module, Counter::GATE_OUTPUT));
	addOutput(createOutput<PJ301MPort>(Vec(10, 235), module, Counter::START_OUTPUT));
	addOutput(createOutput<PJ301MPort>(Vec(10, 270), module, Counter::STOP_OUTPUT));

}
