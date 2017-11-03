#include "ML_modules.hpp"

#include "dsp/digital.hpp"

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
	enum LightIds {
		STEP1_LIGHT,
		STEP2_LIGHT,
		STEP3_LIGHT,
		STEP4_LIGHT,
		STEP5_LIGHT,
		STEP6_LIGHT,
		STEP7_LIGHT,
		STEP8_LIGHT,
		NUM_LIGHTS
	};

	SeqSwitch() : Module( NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS ) {};
	void step() override;

	int position=0;

#ifdef v040
	float stepLights[8] = {};
#endif


        const float in_min[4] = {0.0, 0.0, 0.0, -5.0};
        const float in_max[4] = {8.0, 6.0, 10.0, 5.0};


	SchmittTrigger upTrigger, downTrigger, resetTrigger, stepTriggers[8];

	enum InputRange {
                Zero_Eight,
		Zero_Six,
                Zero_Ten,
                MinusFive_Five
        };


        json_t *toJson() override {

                json_t *rootJ = json_object();

                // outMode:

                json_object_set_new(rootJ, "inputRange", json_integer(inputRange));

                return rootJ;
        };

        void fromJson(json_t *rootJ) override {

                // outMode:

                json_t *inputRangeJ = json_object_get(rootJ, "inputRange");
                if(inputRangeJ) inputRange = (InputRange) json_integer_value(inputRangeJ);
        };


        InputRange inputRange = Zero_Eight;

#ifdef v_dev
	void reset() override {

		position = 0;
		for(int i=0; i<8; i++) lights[i].value = 0.0;
	};
#endif

#ifdef v040
	void initialize() override {

		position=0;
		for(int i=0; i<8; i++) stepLights[i] = 0.0;
	};
#endif
};


void SeqSwitch::step() {

	float out=0.0;

	int numSteps = round(clampf(params[NUM_STEPS].value,1.0,8.0));
	if( inputs[NUMSTEPS_INPUT].active ) numSteps = round(clampf(inputs[NUMSTEPS_INPUT].value,1.0,8.0));

	if( inputs[POS_INPUT].active ) {

//		position = round( clampf( inputs[POS_INPUT].value,0.0,8.0))/8.0 * (numSteps-1) ; 

                float in_value = clampf( inputs[POS_INPUT].value,in_min[inputRange],in_max[inputRange] );

                position = round( rescalef( in_value, in_min[inputRange], in_max[inputRange], 0, numSteps-1 ) );


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

	out = inputs[IN1_INPUT+position].normalize(0.0);

#ifdef v040
	for(int i=0; i<8; i++) stepLights[i] = (i==position)?1.0:0.0;
#endif
#ifdef v_dev
	for(int i=0; i<8; i++) lights[i].value = (i==position)?1.0:0.0;
#endif


	outputs[OUT1_OUTPUT].value = out;
};

struct SeqSwitchRangeItem : MenuItem {

        SeqSwitch *seqSwitch;
        SeqSwitch::InputRange inputRange;

#ifdef v040
        void onAction() override {
                seqSwitch->inputRange = inputRange;
        };
#endif

#ifdef v_dev
        void onAction(EventAction &e) override {
                seqSwitch->inputRange = inputRange;
        };
#endif

        void step() override {
                rightText = (seqSwitch->inputRange == inputRange)? "✔" : "";
        };

};

Menu *SeqSwitchWidget::createContextMenu() {

        Menu *menu = ModuleWidget::createContextMenu();

        MenuLabel *spacerLabel = new MenuLabel();
        menu->pushChild(spacerLabel);

        SeqSwitch *seqSwitch = dynamic_cast<SeqSwitch*>(module);
        assert(seqSwitch);

        MenuLabel *modeLabel2 = new MenuLabel();
        modeLabel2->text = "Input Range";
        menu->pushChild(modeLabel2);

        SeqSwitchRangeItem *zeroEightItem = new SeqSwitchRangeItem();
        zeroEightItem->text = "0 - 8V";
        zeroEightItem->seqSwitch = seqSwitch;
        zeroEightItem->inputRange = SeqSwitch::Zero_Eight;
        menu->pushChild(zeroEightItem);

        SeqSwitchRangeItem *zeroSixItem = new SeqSwitchRangeItem();
        zeroSixItem->text = "0 - 6V";
        zeroSixItem->seqSwitch = seqSwitch;
        zeroSixItem->inputRange = SeqSwitch::Zero_Six;
        menu->pushChild(zeroSixItem);

        SeqSwitchRangeItem *zeroTenItem = new SeqSwitchRangeItem();
        zeroTenItem->text = "0 - 10V";
        zeroTenItem->seqSwitch = seqSwitch;
        zeroTenItem->inputRange = SeqSwitch::Zero_Ten;
        menu->pushChild(zeroTenItem);

        SeqSwitchRangeItem *fiveFiveItem = new SeqSwitchRangeItem();
        fiveFiveItem->text = "-5 - 5V";
        fiveFiveItem->seqSwitch = seqSwitch;
        fiveFiveItem->inputRange = SeqSwitch::MinusFive_Five;;
        menu->pushChild(fiveFiveItem);

        return menu;
};


SeqSwitchWidget::SeqSwitchWidget() {

	SeqSwitch *module = new SeqSwitch();
	setModule(module);
	box.size = Vec(15*8, 380);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin,"res/SeqSwitch.svg")));
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

#ifdef v040
	addChild(createValueLight<SmallLight<GreenValueLight>>(Vec(17, offset_y + 8 + 0*delta_y), &module->stepLights[0]));
	addChild(createValueLight<SmallLight<GreenValueLight>>(Vec(17, offset_y + 8 + 1*delta_y), &module->stepLights[1]));
	addChild(createValueLight<SmallLight<GreenValueLight>>(Vec(17, offset_y + 8 + 2*delta_y), &module->stepLights[2]));
	addChild(createValueLight<SmallLight<GreenValueLight>>(Vec(17, offset_y + 8 + 3*delta_y), &module->stepLights[3]));

	addChild(createValueLight<SmallLight<GreenValueLight>>(Vec(94, offset_y + 8 + 0*delta_y), &module->stepLights[4]));
	addChild(createValueLight<SmallLight<GreenValueLight>>(Vec(94, offset_y + 8 + 1*delta_y), &module->stepLights[5]));
	addChild(createValueLight<SmallLight<GreenValueLight>>(Vec(94, offset_y + 8 + 2*delta_y), &module->stepLights[6]));
	addChild(createValueLight<SmallLight<GreenValueLight>>(Vec(94, offset_y + 8 + 3*delta_y), &module->stepLights[7]));
#endif

#ifdef v_dev
	addChild(createLight<SmallLight<GreenLight>>(Vec(17, offset_y + 8 + 0*delta_y), module, SeqSwitch::STEP1_LIGHT));
	addChild(createLight<SmallLight<GreenLight>>(Vec(17, offset_y + 8 + 1*delta_y), module, SeqSwitch::STEP2_LIGHT));
	addChild(createLight<SmallLight<GreenLight>>(Vec(17, offset_y + 8 + 2*delta_y), module, SeqSwitch::STEP3_LIGHT));
	addChild(createLight<SmallLight<GreenLight>>(Vec(17, offset_y + 8 + 3*delta_y), module, SeqSwitch::STEP4_LIGHT));

	addChild(createLight<SmallLight<GreenLight>>(Vec(94, offset_y + 8 + 0*delta_y), module, SeqSwitch::STEP5_LIGHT));
	addChild(createLight<SmallLight<GreenLight>>(Vec(94, offset_y + 8 + 1*delta_y), module, SeqSwitch::STEP6_LIGHT));
	addChild(createLight<SmallLight<GreenLight>>(Vec(94, offset_y + 8 + 2*delta_y), module, SeqSwitch::STEP7_LIGHT));
	addChild(createLight<SmallLight<GreenLight>>(Vec(94, offset_y + 8 + 3*delta_y), module, SeqSwitch::STEP8_LIGHT));
#endif
	addInput(createInput<PJ301MPort>(Vec(20, 320),    module, SeqSwitch::POS_INPUT));
	addOutput(createOutput<PJ301MPort>(Vec(76, 320), module, SeqSwitch::OUT1_OUTPUT));

}
