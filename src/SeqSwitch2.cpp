#include "ML_modules.hpp"

#include "dsp/digital.hpp"

struct SeqSwitch2 : Module {
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
		TRIGUP_INPUT,
		TRIGDN_INPUT,
		RESET_INPUT,
		NUMSTEPS_INPUT,
		POS_INPUT,
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

	SeqSwitch2() : Module( NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS ) {};

	void step() override;

	int position=0;

	float stepLights[8] = {};
	float outs[8] = {};

	SchmittTrigger upTrigger, downTrigger, resetTrigger, stepTriggers[8];

#ifdef v_dev
	void reset() override {
#endif

#ifdef v040
	void initialize() override {
#endif
		position=0;
		for(int i=0; i<8; i++) stepLights[i] = 0.0;
	};

	enum OutMode {
		ZERO,
		LAST,
	};

	OutMode outMode = ZERO;

	json_t *toJson() override {

		json_t *rootJ = json_object();

		// outMode:
	
		json_object_set_new(rootJ, "outMode", json_integer(outMode));

		return rootJ;
	};
	
	void fromJson(json_t *rootJ) override {

		// outMode:

		json_t *outModeJ = json_object_get(rootJ, "outMode");
		if(outModeJ) outMode = (OutMode) json_integer_value(outModeJ);
	};


};



void SeqSwitch2::step() {

	if(outMode==ZERO) { for(int i=0; i<8; i++) outs[i]=0.0; }

	int numSteps = round(clampf(params[NUM_STEPS].value,1.0,8.0));
	if( inputs[NUMSTEPS_INPUT].active ) numSteps = round(clampf(inputs[NUMSTEPS_INPUT].value,1.0,8.0));

	if( inputs[POS_INPUT].active ) {

		position = round( clampf( inputs[POS_INPUT].value,0.0,8.0))/8.0 * (numSteps-1) ; 

	} else {

		if( inputs[TRIGUP_INPUT].active ) {
			if (upTrigger.process(inputs[TRIGUP_INPUT].value) ) position++;
		}

		if( inputs[TRIGDN_INPUT].active ) {
			if (downTrigger.process(inputs[TRIGDN_INPUT].value) ) position--;
		}

		if( inputs[RESET_INPUT].active ) {
			if (resetTrigger.process(inputs[RESET_INPUT].value) ) position = 0;
		}

	};


	for(int i=0; i<numSteps; i++) {
		if( stepTriggers[i].process(params[STEP1_PARAM+i].value)) position = i;
	};

	while( position < 0 )         position += numSteps;
	while( position >= numSteps ) position -= numSteps;

	outs[position] = inputs[IN_INPUT].normalize(0.0);

	for(int i=0; i<8; i++) stepLights[i] = (i==position)?1.0:0.0;

	for(int i=0; i<8; i++) outputs[OUT1_OUTPUT+i].value = outs[i];
};



SeqSwitch2Widget::SeqSwitch2Widget() {

	SeqSwitch2 *module = new SeqSwitch2();
	setModule(module);
	box.size = Vec(15*8, 380);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin,"res/SeqSwitch2.svg")));
		addChild(panel);
	}

	addChild(createScrew<ScrewSilver>(Vec(15, 0)));
	addChild(createScrew<ScrewSilver>(Vec(15, 365)));

	addParam(createParam<Davies1900hSmallBlackKnob>(Vec(21,  66), module, SeqSwitch2::NUM_STEPS, 1.0, 8.0, 8.0));

	addInput(createInput<PJ301MPort>(Vec(76, 66),    module, SeqSwitch2::NUMSTEPS_INPUT));

	addInput(createInput<PJ301MPort>(Vec(10, 272),    module, SeqSwitch2::TRIGUP_INPUT));
	addInput(createInput<PJ301MPort>(Vec(50, 272),    module, SeqSwitch2::RESET_INPUT));
	addInput(createInput<PJ301MPort>(Vec(86, 272),    module, SeqSwitch2::TRIGDN_INPUT));

	const float offset_y = 118, delta_y=38;

	addOutput(createOutput<PJ301MPort>(Vec(31, offset_y + 0*delta_y),    module, SeqSwitch2::OUT1_OUTPUT));
	addOutput(createOutput<PJ301MPort>(Vec(31, offset_y + 1*delta_y),    module, SeqSwitch2::OUT2_OUTPUT));
	addOutput(createOutput<PJ301MPort>(Vec(31, offset_y + 2*delta_y),    module, SeqSwitch2::OUT3_OUTPUT));
	addOutput(createOutput<PJ301MPort>(Vec(31, offset_y + 3*delta_y),    module, SeqSwitch2::OUT4_OUTPUT));

	addOutput(createOutput<PJ301MPort>(Vec(64, offset_y + 0*delta_y),    module, SeqSwitch2::OUT5_OUTPUT));
	addOutput(createOutput<PJ301MPort>(Vec(64, offset_y + 1*delta_y),    module, SeqSwitch2::OUT6_OUTPUT));
	addOutput(createOutput<PJ301MPort>(Vec(64, offset_y + 2*delta_y),    module, SeqSwitch2::OUT7_OUTPUT));
	addOutput(createOutput<PJ301MPort>(Vec(64, offset_y + 3*delta_y),    module, SeqSwitch2::OUT8_OUTPUT));

	addParam(createParam<LEDButton>(Vec(12, offset_y + 3 + 0*delta_y), module, SeqSwitch2::STEP1_PARAM, 0.0, 1.0, 0.0));
	addParam(createParam<LEDButton>(Vec(12, offset_y + 3 + 1*delta_y), module, SeqSwitch2::STEP2_PARAM, 0.0, 1.0, 0.0));
	addParam(createParam<LEDButton>(Vec(12, offset_y + 3 + 2*delta_y), module, SeqSwitch2::STEP3_PARAM, 0.0, 1.0, 0.0));
	addParam(createParam<LEDButton>(Vec(12, offset_y + 3 + 3*delta_y), module, SeqSwitch2::STEP4_PARAM, 0.0, 1.0, 0.0));

	addParam(createParam<LEDButton>(Vec(89, offset_y + 3 + 0*delta_y), module, SeqSwitch2::STEP5_PARAM, 0.0, 1.0, 0.0));
	addParam(createParam<LEDButton>(Vec(89, offset_y + 3 + 1*delta_y), module, SeqSwitch2::STEP6_PARAM, 0.0, 1.0, 0.0));
	addParam(createParam<LEDButton>(Vec(89, offset_y + 3 + 2*delta_y), module, SeqSwitch2::STEP7_PARAM, 0.0, 1.0, 0.0));
	addParam(createParam<LEDButton>(Vec(89, offset_y + 3 + 3*delta_y), module, SeqSwitch2::STEP8_PARAM, 0.0, 1.0, 0.0));

	addChild(createValueLight<SmallLight<GreenValueLight>>(Vec(17, offset_y + 8 + 0*delta_y), &module->stepLights[0]));
	addChild(createValueLight<SmallLight<GreenValueLight>>(Vec(17, offset_y + 8 + 1*delta_y), &module->stepLights[1]));
	addChild(createValueLight<SmallLight<GreenValueLight>>(Vec(17, offset_y + 8 + 2*delta_y), &module->stepLights[2]));
	addChild(createValueLight<SmallLight<GreenValueLight>>(Vec(17, offset_y + 8 + 3*delta_y), &module->stepLights[3]));

	addChild(createValueLight<SmallLight<GreenValueLight>>(Vec(94, offset_y + 8 + 0*delta_y), &module->stepLights[4]));
	addChild(createValueLight<SmallLight<GreenValueLight>>(Vec(94, offset_y + 8 + 1*delta_y), &module->stepLights[5]));
	addChild(createValueLight<SmallLight<GreenValueLight>>(Vec(94, offset_y + 8 + 2*delta_y), &module->stepLights[6]));
	addChild(createValueLight<SmallLight<GreenValueLight>>(Vec(94, offset_y + 8 + 3*delta_y), &module->stepLights[7]));

	addInput(createInput<PJ301MPort>(Vec(20, 320),    module, SeqSwitch2::POS_INPUT));
	addInput(createInput<PJ301MPort>(Vec(76, 320), module, SeqSwitch2::IN_INPUT));

};

struct SeqSwitch2OutModeItem : MenuItem {

	SeqSwitch2 *seqSwitch2;
	SeqSwitch2::OutMode outMode;

	void onAction() override {
		seqSwitch2->outMode = outMode;
	};

	void step() override {
		rightText = (seqSwitch2->outMode == outMode)? "✔" : "";
	};


};

Menu *SeqSwitch2Widget::createContextMenu() {

	Menu *menu = ModuleWidget::createContextMenu();

	MenuLabel *spacerLabel = new MenuLabel();
	menu->pushChild(spacerLabel);

	SeqSwitch2 *seqSwitch2 = dynamic_cast<SeqSwitch2*>(module);
	assert(seqSwitch2);

	MenuLabel *modeLabel = new MenuLabel();
	modeLabel->text = "Output Mode";
	menu->pushChild(modeLabel);

	SeqSwitch2OutModeItem *zeroItem = new SeqSwitch2OutModeItem();
	zeroItem->text = "Zero";	
	zeroItem->seqSwitch2 = seqSwitch2;
	zeroItem->outMode = SeqSwitch2::ZERO;
	menu->pushChild(zeroItem);

	SeqSwitch2OutModeItem *lastItem = new SeqSwitch2OutModeItem();
	lastItem->text = "Last";	
	lastItem->seqSwitch2 = seqSwitch2;
	lastItem->outMode = SeqSwitch2::LAST;
	menu->pushChild(lastItem);

	return menu;
};
