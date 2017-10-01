#include "ML_modules.hpp"


struct SeqSwitch : Module {
	enum ParamIds {
		NUM_STEPS,
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
	int position;



	SchmittTrigger upTrigger, downTrigger;

	void initialize(){
		position=0;
//		numSteps=1;
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

	if( inputs[POS_INPUT] ) {

		position = round( clampf( (getf(inputs[POS_INPUT])+5.0)/10.0,0.0,1.0))  * numSteps;

	} else {

		if( inputs[TRIGUP_INPUT] ) {
			if (upTrigger.process(*inputs[TRIGUP_INPUT]) ) position++;
		}

		if( inputs[TRIGDN_INPUT] ) {
			if (downTrigger.process(*inputs[TRIGDN_INPUT]) ) position--;
		}

	};

	while( position < 0 )        position += numSteps;
	while( position >= numSteps ) position -= numSteps;


	if( inputs[IN1_INPUT+position] ) {
		out = getf(inputs[IN1_INPUT+position]);
	} else {
		out = 0.0;
	};


	setf(outputs[OUT1_OUTPUT],out);
};



SeqSwitchWidget::SeqSwitchWidget() {

	SeqSwitch *module = new SeqSwitch();
	setModule(module);
	box.size = Vec(15*6, 380);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
	//	panel->setBackground(SVG::load(assetPlugin(plugin,"res/TrigBuf.svg")));
		panel->setBackground(SVG::load("plugins/ML_modules/res/SeqSwitch.svg"));
		addChild(panel);
	}

	addChild(createScrew<ScrewSilver>(Vec(15, 0)));
	addChild(createScrew<ScrewSilver>(Vec(15, 365)));

	addParam(createParam<Davies1900hSmallBlackKnob>(Vec(10,  60), module, SeqSwitch::NUM_STEPS, 1.0, 8.0, 8.0));

	addInput(createInput<PJ301MPort>(Vec(10, 90),    module, SeqSwitch::TRIGUP_INPUT));
	addInput(createInput<PJ301MPort>(Vec(40, 90),    module, SeqSwitch::TRIGDN_INPUT));

	addInput(createInput<PJ301MPort>(Vec(10, 140),    module, SeqSwitch::IN1_INPUT));
	addInput(createInput<PJ301MPort>(Vec(10, 180),    module, SeqSwitch::IN2_INPUT));
	addInput(createInput<PJ301MPort>(Vec(10, 220),    module, SeqSwitch::IN3_INPUT));
	addInput(createInput<PJ301MPort>(Vec(10, 260),    module, SeqSwitch::IN4_INPUT));
	addInput(createInput<PJ301MPort>(Vec(40, 140),    module, SeqSwitch::IN5_INPUT));
	addInput(createInput<PJ301MPort>(Vec(40, 180),    module, SeqSwitch::IN6_INPUT));
	addInput(createInput<PJ301MPort>(Vec(40, 220),    module, SeqSwitch::IN7_INPUT));
	addInput(createInput<PJ301MPort>(Vec(40, 260),    module, SeqSwitch::IN8_INPUT));


	addInput(createInput<PJ301MPort>(Vec(40, 60),    module, SeqSwitch::POS_INPUT));

	addOutput(createOutput<PJ301MPort>(Vec(30, 340), module, SeqSwitch::OUT1_OUTPUT));

//	addChild(createValueLight<SmallLight<GreenValueLight>>(Vec(46, 222), &module->arm2));
}
