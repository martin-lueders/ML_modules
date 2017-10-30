#include "ML_modules.hpp"

#include "dsp/digital.hpp"

#include <cmath>
#include <cstdlib>

#ifdef TEST

struct ShiftRegister2 : Module {
	enum ParamIds {
		NUM_STEPS_PARAM,
		PROB1_PARAM,
		PROB2_PARAM,
		MIX1_PARAM,
		AUX_OFFSET_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		IN1_INPUT,
		IN2_INPUT,
		TRIGGER_INPUT,
		NUM_STEPS_INPUT,
		PROB1_INPUT,
		PROB2_INPUT,
		MIX1_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT_OUTPUT,
		AUX_OUTPUT,
		NUM_OUTPUTS
	};

	ShiftRegister2() : Module( NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS ) {};


	void step() override;


	float values[32] = {};

	SchmittTrigger trigTrigger;

	inline float randf() {return rand()/(RAND_MAX-1.0);}

#ifdef v_dev
	void reset() override {
#endif

#ifdef v040
	void initialize() override {
#endif
	};

};




void ShiftRegister2::step() {

	if( inputs[TRIGGER_INPUT].active ) {

		if( trigTrigger.process(inputs[TRIGGER_INPUT].value) ) {

			float new_in1 = inputs[IN1_INPUT].normalize( randf()*20.0-10.0 );
			float new_in2 = inputs[IN2_INPUT].normalize( 0.0 );

			for(int i=32; i>0; i--) values[i] = values[i-1];

			float p1 = params[PROB1_PARAM].value + inputs[PROB1_INPUT].normalize(0.0);
			float p2 = params[PROB2_PARAM].value + inputs[PROB2_INPUT].normalize(0.0);

			bool replace = ( randf() < p1 );
			bool rnd2 = ( randf() < p2 );

			float a = params[MIX1_PARAM].value;

			int num_steps = roundf(params[NUM_STEPS_PARAM].value);

			if(replace) {
				values[0] = a* (rnd2?new_in1:new_in2) + (1-a)*values[num_steps];
			} else {
				values[0] = values[num_steps];
			};

		};

	};

	outputs[OUT_OUTPUT].value = values[0];

	int offset = roundf(params[AUX_OFFSET_PARAM].value);

	outputs[AUX_OUTPUT].value = values[offset];
};



ShiftRegister2Widget::ShiftRegister2Widget() {

	ShiftRegister2 *module = new ShiftRegister2();
	setModule(module);
	box.size = Vec(15*8, 380);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin,"res/ShiftReg2.svg")));

		addChild(panel);
	}

	addChild(createScrew<ScrewSilver>(Vec(15, 0)));
	addChild(createScrew<ScrewSilver>(Vec(15, 365)));




	const float offset_y = 140, delta_y = 26, offset_x=12;




	addInput(createInput<PJ301MPort>(Vec(20,  40), module, ShiftRegister2::IN1_INPUT));
	addInput(createInput<PJ301MPort>(Vec(60,  40), module, ShiftRegister2::IN2_INPUT));

	addInput(createInput<PJ301MPort>(Vec(20,  80), module, ShiftRegister2::TRIGGER_INPUT));

	addInput(createInput<PJ301MPort>(Vec(20,  120), module, ShiftRegister2::NUM_STEPS_INPUT));
	addInput(createInput<PJ301MPort>(Vec(20,  160), module, ShiftRegister2::PROB1_INPUT));
	addInput(createInput<PJ301MPort>(Vec(20,  200), module, ShiftRegister2::PROB2_INPUT));
	addInput(createInput<PJ301MPort>(Vec(20,  240), module, ShiftRegister2::MIX1_INPUT));

        addParam(createParam<Davies1900hSmallBlackKnob>(Vec(60,  120), module, ShiftRegister2::NUM_STEPS_PARAM, 1.0, 16.0, 8.0));
        addParam(createParam<Davies1900hSmallBlackKnob>(Vec(60,  160), module, ShiftRegister2::PROB1_PARAM, 0.0, 1.0, 0.0));
        addParam(createParam<Davies1900hSmallBlackKnob>(Vec(60,  200), module, ShiftRegister2::PROB2_PARAM, 0.0, 1.0, 0.0));
        addParam(createParam<Davies1900hSmallBlackKnob>(Vec(60,  240), module, ShiftRegister2::MIX1_PARAM, 0.0, 1.0, 0.0));

        addParam(createParam<Davies1900hSmallBlackKnob>(Vec(20,  320), module, ShiftRegister2::AUX_OFFSET_PARAM, 1.0, 16.0, 1.0));


	addOutput(createOutput<PJ301MPort>(Vec(61, 280 ),    module, ShiftRegister2::OUT_OUTPUT));
	addOutput(createOutput<PJ301MPort>(Vec(61, 320 ),    module, ShiftRegister2::AUX_OUTPUT));
}

#endif
