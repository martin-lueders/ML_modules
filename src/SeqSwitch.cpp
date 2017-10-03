#include "ML_modules.hpp"


struct SeqSwitch : Module {
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
		IN1_INPUT,
		IN2_INPUT,
		IN3_INPUT,
		IN4_INPUT,
		IN5_INPUT,
		IN6_INPUT,
		IN7_INPUT,
		IN8_INPUT,
		POS_INPUT,
		TRIGUP_INPUT,
		TRIGDN_INPUT,
		RESET_INPUT,
		NUMSTEPS_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT1_OUTPUT,
		NUM_OUTPUTS
	};

//	SeqSwitch() : Module( NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS ) {};
	SeqSwitch() ;

	void step();

//	int numSteps;
	int position=0;

	float stepLights[8] = {};

	SchmittTrigger upTrigger, downTrigger, resetTrigger, stepTriggers[8];

	void initialize(){
		position=0;
		for(int i=0; i<8; i++) stepLights[i] = 0.0;
	};

};

SeqSwitch::SeqSwitch() {
	params.resize(NUM_PARAMS);
	inputs.resize(NUM_INPUTS);
	outputs.resize(NUM_OUTPUTS);
};



void SeqSwitch::step() {

	float out=0.0;

	int numSteps = round(clampf(params[NUM_STEPS],1.0,8.0));

	if( inputs[NUMSTEPS_INPUT] ) numSteps = round(clampf(getf(inputs[NUMSTEPS_INPUT]),1.0,8.0));

	if( inputs[POS_INPUT] ) {

		position = round( clampf( getf(inputs[POS_INPUT]),0.0,8.0))/8.0 * (numSteps-1) ; 

	} else {

		if( inputs[TRIGUP_INPUT] ) {
			if (upTrigger.process(*inputs[TRIGUP_INPUT]) ) position++;
		}

		if( inputs[TRIGDN_INPUT] ) {
			if (downTrigger.process(*inputs[TRIGDN_INPUT]) ) position--;
		}

		if( inputs[RESET_INPUT] ) {
			if (resetTrigger.process(*inputs[RESET_INPUT]) ) position = 0;
		}

	};

	for(int i=0; i<numSteps; i++) {

		if( stepTriggers[i].process(params[STEP1_PARAM+i])) position = i;
	};

	while( position < 0 )        position += numSteps;
	while( position >= numSteps ) position -= numSteps;


	if( inputs[IN1_INPUT+position] ) {
		out = getf(inputs[IN1_INPUT+position]);
	} else {
		out = 0.0;
	};

	for(int i=0; i<8; i++) stepLights[i] = (i==position)?1.0:0.0;

	setf(outputs[OUT1_OUTPUT],out);
};



SeqSwitchWidget::SeqSwitchWidget() {

	SeqSwitch *module = new SeqSwitch();
	setModule(module);
	box.size = Vec(15*8, 380);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
	//	panel->setBackground(SVG::load(assetPlugin(plugin,"res/TrigBuf.svg")));
		panel->setBackground(SVG::load("plugins/ML_modules/res/SeqSwitch.svg"));
		addChild(panel);
	}

	addChild(createScrew<ScrewSilver>(Vec(15, 0)));
	addChild(createScrew<ScrewSilver>(Vec(15, 365)));

	addParam(createParam<Davies1900hSmallBlackKnob>(Vec(21,  66), module, SeqSwitch::NUM_STEPS, 1.0, 8.0, 8.0));

	addInput(createInput<PJ301MPort>(Vec(76, 66),    module, SeqSwitch::NUMSTEPS_INPUT));

	addInput(createInput<PJ301MPort>(Vec(10, 272),    module, SeqSwitch::TRIGUP_INPUT));
	addInput(createInput<PJ301MPort>(Vec(50, 272),    module, SeqSwitch::RESET_INPUT));
	addInput(createInput<PJ301MPort>(Vec(86, 272),    module, SeqSwitch::TRIGDN_INPUT));

	const float offset_y = 118, delta_y=38;

	addInput(createInput<PJ301MPort>(Vec(31, offset_y + 0*delta_y),    module, SeqSwitch::IN1_INPUT));
	addInput(createInput<PJ301MPort>(Vec(31, offset_y + 1*delta_y),    module, SeqSwitch::IN2_INPUT));
	addInput(createInput<PJ301MPort>(Vec(31, offset_y + 2*delta_y),    module, SeqSwitch::IN3_INPUT));
	addInput(createInput<PJ301MPort>(Vec(31, offset_y + 3*delta_y),    module, SeqSwitch::IN4_INPUT));

	addInput(createInput<PJ301MPort>(Vec(64, offset_y + 0*delta_y),    module, SeqSwitch::IN5_INPUT));
	addInput(createInput<PJ301MPort>(Vec(64, offset_y + 1*delta_y),    module, SeqSwitch::IN6_INPUT));
	addInput(createInput<PJ301MPort>(Vec(64, offset_y + 2*delta_y),    module, SeqSwitch::IN7_INPUT));
	addInput(createInput<PJ301MPort>(Vec(64, offset_y + 3*delta_y),    module, SeqSwitch::IN8_INPUT));

	addParam(createParam<LEDButton>(Vec(12, offset_y + 3 + 0*delta_y), module, SeqSwitch::STEP1_PARAM, 0.0, 1.0, 0.0));
	addParam(createParam<LEDButton>(Vec(12, offset_y + 3 + 1*delta_y), module, SeqSwitch::STEP2_PARAM, 0.0, 1.0, 0.0));
	addParam(createParam<LEDButton>(Vec(12, offset_y + 3 + 2*delta_y), module, SeqSwitch::STEP3_PARAM, 0.0, 1.0, 0.0));
	addParam(createParam<LEDButton>(Vec(12, offset_y + 3 + 3*delta_y), module, SeqSwitch::STEP4_PARAM, 0.0, 1.0, 0.0));

	addParam(createParam<LEDButton>(Vec(89, offset_y + 3 + 0*delta_y), module, SeqSwitch::STEP5_PARAM, 0.0, 1.0, 0.0));
	addParam(createParam<LEDButton>(Vec(89, offset_y + 3 + 1*delta_y), module, SeqSwitch::STEP6_PARAM, 0.0, 1.0, 0.0));
	addParam(createParam<LEDButton>(Vec(89, offset_y + 3 + 2*delta_y), module, SeqSwitch::STEP7_PARAM, 0.0, 1.0, 0.0));
	addParam(createParam<LEDButton>(Vec(89, offset_y + 3 + 3*delta_y), module, SeqSwitch::STEP8_PARAM, 0.0, 1.0, 0.0));

	addChild(createValueLight<SmallLight<GreenValueLight>>(Vec(17, offset_y + 8 + 0*delta_y), &module->stepLights[0]));
	addChild(createValueLight<SmallLight<GreenValueLight>>(Vec(17, offset_y + 8 + 1*delta_y), &module->stepLights[1]));
	addChild(createValueLight<SmallLight<GreenValueLight>>(Vec(17, offset_y + 8 + 2*delta_y), &module->stepLights[2]));
	addChild(createValueLight<SmallLight<GreenValueLight>>(Vec(17, offset_y + 8 + 3*delta_y), &module->stepLights[3]));

	addChild(createValueLight<SmallLight<GreenValueLight>>(Vec(94, offset_y + 8 + 0*delta_y), &module->stepLights[4]));
	addChild(createValueLight<SmallLight<GreenValueLight>>(Vec(94, offset_y + 8 + 1*delta_y), &module->stepLights[5]));
	addChild(createValueLight<SmallLight<GreenValueLight>>(Vec(94, offset_y + 8 + 2*delta_y), &module->stepLights[6]));
	addChild(createValueLight<SmallLight<GreenValueLight>>(Vec(94, offset_y + 8 + 3*delta_y), &module->stepLights[7]));

	addInput(createInput<PJ301MPort>(Vec(20, 320),    module, SeqSwitch::POS_INPUT));
	addOutput(createOutput<PJ301MPort>(Vec(76, 320), module, SeqSwitch::OUT1_OUTPUT));

}
