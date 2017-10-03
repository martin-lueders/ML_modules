#include "ML_modules.hpp"


struct ShiftRegister : Module {
	enum ParamIds {
		NUM_STEPS,
		STEP1_PARAM,
		STEP2_PARAM,
		STEP3_PARAM,
		STEP4_PARAM,
		STEP5_PARAM,
		STEP6_PARAM,
		STEP7_PARAM,
		STEP8_PARAM,
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
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		OUT3_OUTPUT,
		OUT4_OUTPUT,
		OUT5_OUTPUT,
		OUT6_OUTPUT,
		OUT7_OUTPUT,
		OUT8_OUTPUT,
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

//	ShiftRegister() : Module( NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS ) {};
	ShiftRegister() ;

	void step();

//	int numSteps;
	int position=0;

	float stepLights[8] = {};
	float values[8] = {};

	SchmittTrigger upTrigger, downTrigger, setTrigger;

	void initialize(){
		position=0;
		for(int i=0; i<8; i++) {
			stepLights[i] = 0.0;
			values[i] = 0.0;
		};
	};

};

ShiftRegister::ShiftRegister() {
	params.resize(NUM_PARAMS);
	inputs.resize(NUM_INPUTS);
	outputs.resize(NUM_OUTPUTS);
};



void ShiftRegister::step() {


	if( inputs[TRIGGER_INPUT] ) {

		if( setTrigger.process(*inputs[TRIGGER_INPUT]) ) {

			for(int i=7; i>0; i--) values[i] = values[i-1];
			values[0] = getf(inputs[IN_INPUT]);

		};

	};



	for(int i=0; i<8; i++) setf(outputs[OUT1_OUTPUT+i],values[i]);

};



ShiftRegisterWidget::ShiftRegisterWidget() {

	ShiftRegister *module = new ShiftRegister();
	setModule(module);
	box.size = Vec(15*4, 380);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
	//	panel->setBackground(SVG::load(assetPlugin(plugin,"res/TrigBuf.svg")));
		panel->setBackground(SVG::load("plugins/ML_modules/res/ShiftReg.svg"));
		addChild(panel);
	}

	addChild(createScrew<ScrewSilver>(Vec(15, 0)));
	addChild(createScrew<ScrewSilver>(Vec(15, 365)));



	//addInput(createInput<PJ301MPort>(Vec(30, 250),    module, SeqSwitch::TRIGUP_INPUT));
	//addInput(createInput<PJ301MPort>(Vec(65, 250),    module, SeqSwitch::TRIGDN_INPUT));

	const float offset_y = 140, delta_y = 26, offset_x=12;

	for( int i=0; i<8; i++) {

		addOutput(createOutput<PJ301MPort>(Vec(offset_x+17, offset_y + i*delta_y  ),    module, ShiftRegister::OUT1_OUTPUT+i));
		addChild(createValueLight<SmallLight<GreenValueLight>>(Vec(offset_x, offset_y + 8 +   i*delta_y), &module->values[i]));
	};


	addInput(createInput<PJ301MPort>(Vec(offset_x+17, 58), module, ShiftRegister::TRIGGER_INPUT));

	addInput(createInput<PJ301MPort>(Vec(offset_x+17, 94), module, ShiftRegister::IN_INPUT));

}
