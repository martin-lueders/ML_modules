#include "ML_modules.hpp"


struct TrigBuf : Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		ARM1_INPUT,
		ARM2_INPUT,
		TRIG1_INPUT,
		TRIG2_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		NUM_OUTPUTS
	};

	TrigBuf();

	void step();

	bool arm[2];

	SchmittTrigger armTrigger[2];
	SchmittTrigger trigTrigger[2];

	void initialize(){
		for(int i=0; i<2; i++) {arm[i]=false;}
	};

};


TrigBuf::TrigBuf() {
	params.resize(NUM_PARAMS);
	inputs.resize(NUM_INPUTS);
	outputs.resize(NUM_OUTPUTS);
}


void TrigBuf::step() {

	float out;

	for(int i=0; i<2; i++) {

		out = 0.0;

		if( inputs[ARM1_INPUT+i] ) {
			if( armTrigger[i].process(*inputs[ARM1_INPUT+i]) ) { arm[i]=true; }
		};

		if( inputs[TRIG1_INPUT+i]) {
			if( trigTrigger[i].process(*inputs[TRIG1_INPUT+i]) ) {

				if(arm[i]) {
					out    = 10.0;
					arm[i] = false;
				};
			};
		};

		setf(outputs[OUT1_OUTPUT+i],out);
	};

};


TrigBufWidget::TrigBufWidget() {

	TrigBuf *module = new TrigBuf();
	setModule(module);
	box.size = Vec(15*3, 380);

	{
		Panel *panel = new LightPanel();
		panel->box.size = box.size;
		panel->backgroundImage = Image::load("plugins/ML_modules/res/Quantizer.png");
		addChild(panel);
	}

	addChild(createScrew<ScrewSilver>(Vec(15, 0)));
	addChild(createScrew<ScrewSilver>(Vec(15, 365)));

	addInput(createInput<PJ301MPort>(Vec(10, 50),    module, TrigBuf::ARM1_INPUT));
	addInput(createInput<PJ301MPort>(Vec(10, 90),    module, TrigBuf::TRIG1_INPUT));
	addOutput(createOutput<PJ301MPort>(Vec(10, 130), module, TrigBuf::OUT1_OUTPUT));

	addInput(createInput<PJ301MPort>(Vec(10, 200),   module, TrigBuf::ARM2_INPUT));
	addInput(createInput<PJ301MPort>(Vec(10, 240),   module, TrigBuf::TRIG2_INPUT));
	addOutput(createOutput<PJ301MPort>(Vec(10, 280), module, TrigBuf::OUT2_OUTPUT));
}
