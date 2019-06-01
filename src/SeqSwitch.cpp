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

	SeqSwitch() { 
		config( NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS ) ;  
        configParam(SeqSwitch::NUM_STEPS, 1.0, 8.0, 8.0);
        configParam(SeqSwitch::STEP1_PARAM, 0.0, 1.0, 0.0);
        configParam(SeqSwitch::STEP2_PARAM, 0.0, 1.0, 0.0);
        configParam(SeqSwitch::STEP3_PARAM, 0.0, 1.0, 0.0);
        configParam(SeqSwitch::STEP4_PARAM, 0.0, 1.0, 0.0);
        configParam(SeqSwitch::STEP5_PARAM, 0.0, 1.0, 0.0);
        configParam(SeqSwitch::STEP6_PARAM, 0.0, 1.0, 0.0);
        configParam(SeqSwitch::STEP7_PARAM, 0.0, 1.0, 0.0);
        configParam(SeqSwitch::STEP8_PARAM, 0.0, 1.0, 0.0);

		onReset(); 
	};

	void process(const ProcessArgs &args) override;

	int position=0;



        const float in_min[4] = {0.0, 0.0, 0.0, -5.0};
        const float in_max[4] = {8.0, 6.0, 10.0, 5.0};


	dsp::SchmittTrigger upTrigger, downTrigger, resetTrigger, stepTriggers[8];

	enum InputRange {
                Zero_Eight,
		Zero_Six,
                Zero_Ten,
                MinusFive_Five
        };


        json_t *dataToJson() override {

                json_t *rootJ = json_object();

                // outMode:

                json_object_set_new(rootJ, "inputRange", json_integer(inputRange));

                return rootJ;
        };

        void dataFromJson(json_t *rootJ) override {

                // outMode:

                json_t *inputRangeJ = json_object_get(rootJ, "inputRange");
                if(inputRangeJ) inputRange = (InputRange) json_integer_value(inputRangeJ);
        };


        InputRange inputRange = Zero_Eight;

	void onReset() override {

		position = 0;
		for(int i=0; i<8; i++) lights[i].value = 0.0;
	};

};


void SeqSwitch::process(const ProcessArgs &args) {

	float out[PORT_MAX_CHANNELS];
	int in_channels = 0;

	memset(out, 0, PORT_MAX_CHANNELS*sizeof(float));

	int numSteps = round(clamp(params[NUM_STEPS].getValue(),1.0f,8.0f));
	if( inputs[NUMSTEPS_INPUT].isConnected() ) numSteps = round(clamp(inputs[NUMSTEPS_INPUT].getVoltage(),1.0f,8.0f));

	if( inputs[POS_INPUT].isConnected() ) {

        float in_value = clamp( inputs[POS_INPUT].getVoltage(),in_min[inputRange],in_max[inputRange] );
        position = round( rescale( in_value, in_min[inputRange], in_max[inputRange], 0.0f, 1.0f*(numSteps-1) ) );

	} else {

		if( inputs[TRIGUP_INPUT].isConnected() ) {
			if (upTrigger.process(inputs[TRIGUP_INPUT].getVoltage()) ) position++;
		}

		if( inputs[TRIGDN_INPUT].isConnected() ) {
			if (downTrigger.process(inputs[TRIGDN_INPUT].getVoltage()) ) position--;
		}

		if( inputs[RESET_INPUT].isConnected() ) {
			if (resetTrigger.process(inputs[RESET_INPUT].getVoltage()) ) position = 0;
		}

	};


	for(int i=0; i<numSteps; i++) {
		if( stepTriggers[i].process(params[STEP1_PARAM+i].getValue())) position = i;

	};

	while( position < 0 )         position += numSteps;
	while( position >= numSteps ) position -= numSteps;

	if(inputs[IN1_INPUT+position].isConnected()) {
		in_channels = inputs[IN1_INPUT+position].getChannels();
		inputs[IN1_INPUT+position].readVoltages(out);
	}

	for(int i=0; i<8; i++) lights[i].value = (i==position)?1.0:0.0;


	outputs[OUT1_OUTPUT].setChannels(in_channels);
	outputs[OUT1_OUTPUT].writeVoltages(out);
};

struct SeqSwitchRangeItem : MenuItem {

        SeqSwitch *seqSwitch;
        SeqSwitch::InputRange inputRange;

        void onAction(const event::Action &e) override {
                seqSwitch->inputRange = inputRange;
        };

        void step() override {
                rightText = (seqSwitch->inputRange == inputRange)? "✔" : "";
        };

};


struct SeqSwitchWidget : ModuleWidget {
	SeqSwitchWidget(SeqSwitch *module);
	json_t *dataToJsonData() ;
	void dataFromJsonData(json_t *root) ;
	void appendContextMenu(Menu*) override;
};

void SeqSwitchWidget::appendContextMenu(Menu* menu) {

    
        SeqSwitch *seqSwitch = dynamic_cast<SeqSwitch*>(module);
        assert(seqSwitch);

        MenuLabel *modeLabel2 = new MenuLabel();
        modeLabel2->text = "Input Range";
        menu->addChild(modeLabel2);

        SeqSwitchRangeItem *zeroEightItem = new SeqSwitchRangeItem();
        zeroEightItem->text = "0 - 8V";
        zeroEightItem->seqSwitch = seqSwitch;
        zeroEightItem->inputRange = SeqSwitch::Zero_Eight;
        menu->addChild(zeroEightItem);

        SeqSwitchRangeItem *zeroSixItem = new SeqSwitchRangeItem();
        zeroSixItem->text = "0 - 6V";
        zeroSixItem->seqSwitch = seqSwitch;
        zeroSixItem->inputRange = SeqSwitch::Zero_Six;
        menu->addChild(zeroSixItem);

        SeqSwitchRangeItem *zeroTenItem = new SeqSwitchRangeItem();
        zeroTenItem->text = "0 - 10V";
        zeroTenItem->seqSwitch = seqSwitch;
        zeroTenItem->inputRange = SeqSwitch::Zero_Ten;
        menu->addChild(zeroTenItem);

        SeqSwitchRangeItem *fiveFiveItem = new SeqSwitchRangeItem();
        fiveFiveItem->text = "-5 - 5V";
        fiveFiveItem->seqSwitch = seqSwitch;
        fiveFiveItem->inputRange = SeqSwitch::MinusFive_Five;;
        menu->addChild(fiveFiveItem);
};


SeqSwitchWidget::SeqSwitchWidget(SeqSwitch *module) {
		setModule(module);

	box.size = Vec(15*8, 380);

	{
		SvgPanel *panel = new SvgPanel();
		panel->box.size = box.size;
		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance,"res/SeqSwitch.svg")));
		addChild(panel);
	}

	addChild(createWidget<MLScrew>(Vec(15, 0)));
	addChild(createWidget<MLScrew>(Vec(box.size.x-30, 0)));
	addChild(createWidget<MLScrew>(Vec(15, 365)));
	addChild(createWidget<MLScrew>(Vec(box.size.x-30, 365)));

	addParam(createParam<RedSnapMLKnob>(Vec(14,  63), module, SeqSwitch::NUM_STEPS));

	addInput(createInput<MLPort>(Vec(81, 64), module, SeqSwitch::NUMSTEPS_INPUT));

	addInput(createInput<MLPort>(Vec(9, 272),  module, SeqSwitch::TRIGUP_INPUT));
	addInput(createInput<MLPort>(Vec(47, 272), module, SeqSwitch::RESET_INPUT));
	addInput(createInput<MLPort>(Vec(85, 272), module, SeqSwitch::TRIGDN_INPUT));

	const float offset_y = 118, delta_y=38;

	addInput(createInput<MLPort>(Vec(32, offset_y + 0*delta_y), module, SeqSwitch::IN1_INPUT));
	addInput(createInput<MLPort>(Vec(32, offset_y + 1*delta_y), module, SeqSwitch::IN2_INPUT));
	addInput(createInput<MLPort>(Vec(32, offset_y + 2*delta_y), module, SeqSwitch::IN3_INPUT));
	addInput(createInput<MLPort>(Vec(32, offset_y + 3*delta_y), module, SeqSwitch::IN4_INPUT));

	addInput(createInput<MLPort>(Vec(62, offset_y + 0*delta_y), module, SeqSwitch::IN5_INPUT));
	addInput(createInput<MLPort>(Vec(62, offset_y + 1*delta_y), module, SeqSwitch::IN6_INPUT));
	addInput(createInput<MLPort>(Vec(62, offset_y + 2*delta_y), module, SeqSwitch::IN7_INPUT));
	addInput(createInput<MLPort>(Vec(62, offset_y + 3*delta_y), module, SeqSwitch::IN8_INPUT));

	addParam(createParam<ML_MediumLEDButton>(Vec(11, offset_y + 3 + 0*delta_y), module, SeqSwitch::STEP1_PARAM));
	addParam(createParam<ML_MediumLEDButton>(Vec(11, offset_y + 3 + 1*delta_y), module, SeqSwitch::STEP2_PARAM));
	addParam(createParam<ML_MediumLEDButton>(Vec(11, offset_y + 3 + 2*delta_y), module, SeqSwitch::STEP3_PARAM));
	addParam(createParam<ML_MediumLEDButton>(Vec(11, offset_y + 3 + 3*delta_y), module, SeqSwitch::STEP4_PARAM));

	addParam(createParam<ML_MediumLEDButton>(Vec(89, offset_y + 3 + 0*delta_y), module, SeqSwitch::STEP5_PARAM));
	addParam(createParam<ML_MediumLEDButton>(Vec(89, offset_y + 3 + 1*delta_y), module, SeqSwitch::STEP6_PARAM));
	addParam(createParam<ML_MediumLEDButton>(Vec(89, offset_y + 3 + 2*delta_y), module, SeqSwitch::STEP7_PARAM));
	addParam(createParam<ML_MediumLEDButton>(Vec(89, offset_y + 3 + 3*delta_y), module, SeqSwitch::STEP8_PARAM));

	addChild(createLight<MLMediumLight<GreenLight>>(Vec(15, offset_y + 7 + 0*delta_y), module, SeqSwitch::STEP1_LIGHT));
	addChild(createLight<MLMediumLight<GreenLight>>(Vec(15, offset_y + 7 + 1*delta_y), module, SeqSwitch::STEP2_LIGHT));
	addChild(createLight<MLMediumLight<GreenLight>>(Vec(15, offset_y + 7 + 2*delta_y), module, SeqSwitch::STEP3_LIGHT));
	addChild(createLight<MLMediumLight<GreenLight>>(Vec(15, offset_y + 7 + 3*delta_y), module, SeqSwitch::STEP4_LIGHT));

	addChild(createLight<MLMediumLight<GreenLight>>(Vec(93, offset_y + 7 + 0*delta_y), module, SeqSwitch::STEP5_LIGHT));
	addChild(createLight<MLMediumLight<GreenLight>>(Vec(93, offset_y + 7 + 1*delta_y), module, SeqSwitch::STEP6_LIGHT));
	addChild(createLight<MLMediumLight<GreenLight>>(Vec(93, offset_y + 7 + 2*delta_y), module, SeqSwitch::STEP7_LIGHT));
	addChild(createLight<MLMediumLight<GreenLight>>(Vec(93, offset_y + 7 + 3*delta_y), module, SeqSwitch::STEP8_LIGHT));

	addInput(createInput<MLPort>( Vec(19, 318),  module, SeqSwitch::POS_INPUT));
	addOutput(createOutput<MLPort>(Vec(75, 318), module, SeqSwitch::OUT1_OUTPUT));

}

Model *modelSeqSwitch = createModel<SeqSwitch, SeqSwitchWidget>("SeqSwitch");
