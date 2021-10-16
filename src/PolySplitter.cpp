#include "ML_modules.hpp"
#include "simd_mask.hpp"


using simd::float_4;




struct PolySplitter : Module {
	enum ParamIds {
		SPLIT_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		IN_INPUT,
		NUM_INPUTS = IN_INPUT + 4
	};
	enum OutputIds {
		OUT_A_OUTPUT,
		OUT_B_OUTPUT = OUT_A_OUTPUT + 4,
		NUM_OUTPUTS = OUT_B_OUTPUT + 4
	};
	enum LightIds {
		NUM_LIGHTS
	};



	PolySplitter() {
		config( NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS ); 
		configParam(PolySplitter::SPLIT_PARAM, 0.0, 16.0, 8.0, "Split channel");
		for(int i=0; i<4; i++) {
			configInput(IN_INPUT+i, "A #"+std::to_string(i+1));
			configOutput(OUT_A_OUTPUT+i, "A #"+std::to_string(i+1));
			configOutput(OUT_B_OUTPUT+i, "B #"+std::to_string(i+1));
		}
		onReset(); 
	};

	void onReset() override {channels=0; channels_A = 0; channels_B = 0;};

	void process(const ProcessArgs &args) override;

	int channels;
	int channels_A;
	int channels_B;

};



void PolySplitter::process(const ProcessArgs &args) {

	int split_channel = round(params[SPLIT_PARAM].getValue());
	// float in[PORT_MAX_CHANNELS];

	channels = 0;
	for(int i=0; i<4; i++) channels = MAX(channels, inputs[IN_INPUT+i].getChannels());

	channels_A = MIN(channels, split_channel);
	channels_B = MAX(0, channels-channels_A);


	for(int i=0; i<4; i++) {

		outputs[OUT_A_OUTPUT+i].setChannels(channels_A);
		outputs[OUT_B_OUTPUT+i].setChannels(channels_B);

		for(int c=0; c<channels_A; c++) outputs[OUT_A_OUTPUT+i].setVoltage(inputs[IN_INPUT+i].getNormalPolyVoltage(0.0f, c) , c);
		for(int c=0; c<channels_B; c++) outputs[OUT_B_OUTPUT+i].setVoltage(inputs[IN_INPUT+i].getNormalPolyVoltage(0.0f, channels_A + c) , c);


	}

};


struct PolySplitterWidget : ModuleWidget {
	PolySplitterWidget(PolySplitter *module);
};

PolySplitterWidget::PolySplitterWidget(PolySplitter *module) {

	setModule(module);
	box.size = Vec(15*8, 380);

	{
		SvgPanel *panel = new SvgPanel();
		panel->box.size = box.size;
		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance,"res/PolySplitter.svg")));

		addChild(panel);
	}

	addChild(createWidget<MLScrew>(Vec(15, 0)));
	addChild(createWidget<MLScrew>(Vec(box.size.x-30, 0)));
	addChild(createWidget<MLScrew>(Vec(15, 365)));
	addChild(createWidget<MLScrew>(Vec(box.size.x-30, 365)));


	const float offset_y = 188, delta_y = 32, row1=15, row2 = 48, row3 = 80;

	addParam(createParam<SmallBlueSnapMLKnob>(  Vec(row2-5,  50), module, PolySplitter::SPLIT_PARAM));



	for( int i=0; i<4; i++) {
		addInput(createInput<MLPort>(  Vec(row1, offset_y + i*delta_y ), module, PolySplitter::IN_INPUT+i));
		addOutput(createOutput<MLPortOut>(Vec(row2, offset_y + i*delta_y ), module, PolySplitter::OUT_A_OUTPUT+i));
		addOutput(createOutput<MLPortOut>(Vec(row3, offset_y + i*delta_y ), module, PolySplitter::OUT_B_OUTPUT+i));
	};

	NumberDisplayWidget<int> *display_IN = new NumberDisplayWidget<int>(2);
	display_IN->box.pos = Vec(8,126);
	if(module) display_IN->value = &module->channels;
	addChild(display_IN);

	NumberDisplayWidget<int> *display_A = new NumberDisplayWidget<int>(2);
	display_A->box.pos = Vec(44,126);
	if(module) display_A->value = &module->channels_A;
	addChild(display_A);

	NumberDisplayWidget<int> *display_B = new NumberDisplayWidget<int>(2);
	display_B->box.pos = Vec(80,126);
	if(module) display_B->value = &module->channels_B;
	addChild(display_B);


}

Model *modelPolySplitter = createModel<PolySplitter, PolySplitterWidget>("PolySplitter");
