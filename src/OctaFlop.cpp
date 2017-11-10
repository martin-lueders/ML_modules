#include "ML_modules.hpp"

#include "dsp/digital.hpp"

struct OctaFlop : Module {
	enum ParamIds {
		RESET_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		IN1_INPUT,
		IN2_INPUT,
		IN3_INPUT,
		IN4_INPUT,
		IN5_INPUT,
		IN6_INPUT,
		IN7_INPUT,
		IN8_INPUT,
		TRIG1_INPUT,
		TRIG2_INPUT,
		TRIG3_INPUT,
		TRIG4_INPUT,
		TRIG5_INPUT,
		TRIG6_INPUT,
		TRIG7_INPUT,
		TRIG8_INPUT,
		RESET_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		OUT3_OUTPUT,
		OUT4_OUTPUT,
		OUT5_OUTPUT,
		OUT6_OUTPUT,
		OUT7_OUTPUT,
		OUT8_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightIds {
		STATE1_LIGHT,
		STATE2_LIGHT,
		STATE3_LIGHT,
		STATE4_LIGHT,
		STATE5_LIGHT,
		STATE6_LIGHT,
		STATE7_LIGHT,
		STATE8_LIGHT,
		NUM_LIGHTS
	};

	SchmittTrigger trigger[8], resetTrigger;
	float out[8] = {};
	bool state[8] = {};

#ifdef v040
	OctaFlop() : Module( NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS ) {};
#endif

#ifdef v_dev
	OctaFlop() : Module( NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS ) {};
#endif

	void step() override;

};



void OctaFlop::step() {

	float trig[8];



	trig[0] = inputs[TRIG1_INPUT].normalize(0.0);
	for(int i=1; i<8; i++) trig[i] = inputs[TRIG1_INPUT+i].normalize(10.0-out[i-1]);


	for(int i=0; i<8; i++) {

		if( trigger[i].process(trig[i]) ) {

			state[i] = !state[i];
			out[i]   = state[i]?10.0:0.0;
		};

	}

	if(resetTrigger.process(params[RESET_PARAM].value + inputs[RESET_INPUT].normalize(0.0))) {
		for(int i=0; i<8; i++) {
			state[i]=false;
			out[i] = 0.0;
			trigger[i].reset();
		};
	};


	for(int i=0; i<8; i++) { 
		outputs[OUT1_OUTPUT+i].value = out[i];
#ifdef v_dev
		lights[STATE1_LIGHT+i].value = out[i];
#endif
	}

};



OctaFlopWidget::OctaFlopWidget() {

	OctaFlop *module = new OctaFlop();
	setModule(module);
	box.size = Vec(15*8, 380);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin,"res/OctaFlop.svg")));

		addChild(panel);
	}

	addChild(createScrew<ScrewSilver>(Vec(15, 0)));
	addChild(createScrew<ScrewSilver>(Vec(box.size.x-30, 0)));
	addChild(createScrew<ScrewSilver>(Vec(15, 365)));
	addChild(createScrew<ScrewSilver>(Vec(box.size.x-30, 365)));



	const float offset_y = 62, delta_y = 32, row1=15, row2 = 55, row3 = 80;

	for( int i=0; i<8; i++) {
		addInput(createInput<PJ301MPort>(Vec(row1, offset_y + i*delta_y  ),    module, OctaFlop::TRIG1_INPUT+i));
#ifdef v040
                addChild(createValueLight<SmallLight<GreenValueLight>>(Vec(row2, offset_y + 8 +   i*delta_y), &module->out[i]));
#endif

#ifdef v_dev
                addChild(createLight<SmallLight<GreenLight>>(Vec(row2, offset_y + 8 +   i*delta_y), module,  OctaFlop::STATE1_LIGHT+i));
#endif
		addOutput(createOutput<PJ301MPort>(Vec(row3, offset_y + i*delta_y ), module, OctaFlop::OUT1_OUTPUT+i));
	};

        addParam(createParam<LEDButton>(Vec(row1+3, 320+3), module, OctaFlop::RESET_PARAM, 0.0, 10.0, 0.0));
		addInput(createInput<PJ301MPort>(Vec(row3, 320),    module, OctaFlop::RESET_INPUT));

}
