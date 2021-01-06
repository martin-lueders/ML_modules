#include "ML_modules.hpp"


struct TrigSwitch2 : Module {
	enum ParamIds {
		STEP_PARAM,
		NUM_PARAMS = STEP_PARAM + 9
	};
	enum InputIds {
		CV_INPUT,
		TRIG_INPUT,
		NUM_INPUTS = TRIG_INPUT + 8
	};
	enum OutputIds {
		OUT_OUTPUT,
		NUM_OUTPUTS = OUT_OUTPUT+8
	};
	enum LightIds {
		STEP_LIGHT,
		NUM_LIGHTS = STEP_LIGHT+8
	};


	TrigSwitch2() {
		config( NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS ); 
		for(int i=0; i<8l; i++) configParam(TrigSwitch2::STEP_PARAM + i, 0.0, 1.0, 0.0);
		onReset(); 
	};

	void process(const ProcessArgs &args) override;

	enum OutMode {
		ZERO,
		LAST
	};

	OutMode outMode = ZERO;

	json_t *dataToJson() override {

		json_t *rootJ = json_object();

		// outMode:
	
		json_object_set_new(rootJ, "outMode", json_integer(outMode));
		json_object_set_new(rootJ, "position", json_integer(position));
	

		return rootJ;
	};
	
	void dataFromJson(json_t *rootJ) override {

		// outMode:

		json_t *outModeJ = json_object_get(rootJ, "outMode");
		if(outModeJ) outMode = (OutMode) json_integer_value(outModeJ);
	
		json_t *positionJ = json_object_get(rootJ, "position");
		if(positionJ) position = json_integer_value(positionJ);

	};


	int position=0;


	simd::float_4 outs[8*PORT_MAX_CHANNELS/4] = {};


	dsp::SchmittTrigger stepTriggers[8];


	void onReset() override {

		position = 0;
		for(int i=0; i<8; i++) lights[i].value = 0.0;
		memset(outs, 0, sizeof(outs));
	};

};


void TrigSwitch2::process(const ProcessArgs &args) {


	if(outMode==ZERO) memset(outs, 0, sizeof(outs));

	int channels = inputs[CV_INPUT].getChannels();

	for(int i=0; i<8; i++) outputs[OUT_OUTPUT+i].setChannels(channels);

	for(int i=0; i<8; i++) {
		if( stepTriggers[i].process( inputs[TRIG_INPUT+i].getNormalVoltage(0.0)) + params[STEP_PARAM+i].getValue() ) position = i;
	};

	// outs[position] = inputs[CV_INPUT].getNormalVoltage(0.0);

	if( inputs[CV_INPUT].isConnected()) {
		for(int c=0; c<channels; c+=4) 
			outs[position * PORT_MAX_CHANNELS/4 + c/4 ] = inputs[CV_INPUT].getPolyVoltageSimd<simd::float_4>(c);
	} else {
		memset(outs + position * PORT_MAX_CHANNELS/4, 0, (PORT_MAX_CHANNELS/4)*sizeof(simd::float_4));
	}

	for(int i=0; i<8; i++) {
		lights[i].value = (i==position)?1.0:0.0;
		for(int c=0; c<channels; c+=4) {
			outputs[OUT_OUTPUT+i].setVoltageSimd(outs[i * PORT_MAX_CHANNELS/4 + c/4], c);
		}
	}
	
};



struct TrigSwitch2OutModeItem : MenuItem {

	TrigSwitch2 *trigSwitch2;
	TrigSwitch2::OutMode outMode;

	void onAction(const event::Action &e) override {
		trigSwitch2->outMode = outMode;
	};


	void step() override {
		rightText = (trigSwitch2->outMode == outMode)? "✔" : "";
	};

};


struct TrigSwitch2Widget : ModuleWidget {
	TrigSwitch2Widget(TrigSwitch2 *module);
	json_t *dataToJsonData() ;
	void dataFromJsonData(json_t *root) ;
	void appendContextMenu(Menu*) override;
};

TrigSwitch2Widget::TrigSwitch2Widget(TrigSwitch2 *module) {
	
	setModule(module);

	box.size = Vec(15*8, 380);

	{
		SvgPanel *panel = new SvgPanel();
		panel->box.size = box.size;
		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance,"res/TrigSwitch2.svg")));
		addChild(panel);
	}

	addChild(createWidget<MLScrew>(Vec(15, 0)));
	addChild(createWidget<MLScrew>(Vec(box.size.x-30, 0)));
	addChild(createWidget<MLScrew>(Vec(15, 365)));
	addChild(createWidget<MLScrew>(Vec(box.size.x-30, 365)));


	const float offset_y = 60, delta_y = 32, row1=14, row2 = 50, row3 = 79;

	for (int i=0; i<8; i++) {

		addInput(createInput<MLPort>(             Vec(row1, offset_y + i*delta_y), module, TrigSwitch2::TRIG_INPUT + i));
		addParam(createParam<ML_MediumLEDButton>(Vec(row2 , offset_y + i*delta_y +3 ), module, TrigSwitch2::STEP_PARAM + i));
		addChild(createLight<MLMediumLight<GreenLight>>( Vec(row2 + 4, offset_y + i*delta_y + 7), module, TrigSwitch2::STEP_LIGHT+i));
		addOutput(createOutput<MLPort>(           Vec(row3, offset_y + i*delta_y), module, TrigSwitch2::OUT_OUTPUT + i));

	}
	addInput(createInput<MLPort>(Vec(row3, 320), module, TrigSwitch2::CV_INPUT));

}
void TrigSwitch2Widget::appendContextMenu(Menu *menu) {


	TrigSwitch2 *trigSwitch2 = dynamic_cast<TrigSwitch2*>(module);
	assert(trigSwitch2);

	MenuLabel *modeLabel = new MenuLabel();
	modeLabel->text = "Output Mode";
	menu->addChild(modeLabel);

	TrigSwitch2OutModeItem *zeroItem = new TrigSwitch2OutModeItem();
	zeroItem->text = "Zero";	
	zeroItem->trigSwitch2 = trigSwitch2;
	zeroItem->outMode = TrigSwitch2::ZERO;
	menu->addChild(zeroItem);

	TrigSwitch2OutModeItem *lastItem = new TrigSwitch2OutModeItem();
	lastItem->text = "Last";	
	lastItem->trigSwitch2 = trigSwitch2;
	lastItem->outMode = TrigSwitch2::LAST;
	menu->addChild(lastItem);

};

Model *modelTrigSwitch2 = createModel<TrigSwitch2, TrigSwitch2Widget>("TrigSwitch2");
