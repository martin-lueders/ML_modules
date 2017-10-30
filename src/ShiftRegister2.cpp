#include "ML_modules.hpp"

#include "dsp/digital.hpp"

#ifdef TEST

struct ShiftRegister2 : Module {
	enum ParamIds {
		NUM_STEPS_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		IN_INPUT,
		TRIGGER_INPUT,
		TRIGUP_INPUT,
		TRIGDN_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	ShiftRegister2() : Module( NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS ) {};


	void step() override;

	int position=0;

	float stepLights[8] = {};
	float values[32] = {};

	SchmittTrigger trigTrigger;

#ifdef v_dev
	void reset() override {
#endif

#ifdef v040
	void initialize() override {
#endif
		position=0;
		for(int i=0; i<8; i++) {
			stepLights[i] = 0.0;
			values[i] = 0.0;
		};
	};

};




void ShiftRegister2::step() {

	if( inputs[TRIGGER_INPUT].active ) {

		if( setTrigger.process(inputs[TRIGGER_INPUT].value) ) {

			float new_in = inputs[IN_INPUT].value;
			for(int i=32; i>0; i--) values[i] = values[i-1];
			values[0] = inputs[IN_INPUT].value;

		};

	};

	outputs[OUT_OUTPUT].value = values[0];
};



ShiftRegister2Widget::ShiftRegister2Widget() {

	ShiftRegister2 *module = new ShiftRegister2();
	setModule(module);
	box.size = Vec(15*4, 380);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin,"res/ShiftReg2.svg")));

		addChild(panel);
	}

	addChild(createScrew<ScrewSilver>(Vec(15, 0)));
	addChild(createScrew<ScrewSilver>(Vec(15, 365)));




	const float offset_y = 140, delta_y = 26, offset_x=12;

	for( int i=0; i<8; i++) {

		addOutput(createOutput<PJ301MPort>(Vec(offset_x+17, offset_y + i*delta_y  ),    module, ShiftRegister2::OUT1_OUTPUT+i));
		addChild(createValueLight<SmallLight<GreenValueLight>>(Vec(offset_x, offset_y + 8 +   i*delta_y), &module->values[i]));
	};


	addInput(createInput<PJ301MPort>(Vec(offset_x+17, 58), module, ShiftRegister2::IN_INPUT));
	addInput(createInput<PJ301MPort>(Vec(offset_x+17, 94), module, ShiftRegister2::TRIGGER_INPUT));

}

#endif
