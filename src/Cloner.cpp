#include "ML_modules.hpp"
#include "simd_mask.hpp"

using simd::float_4;


struct Cloner : Module {
	enum ParamIds {
		CHANNELS_PARAM,
		SPREAD_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		CV_INPUT,
		CHANNELS_INPUT,
		SPREAD_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		CV_OUTPUT,
		NUM_OUTPUTS
	};
	enum LighIds {
		NUM_LIGHTS
	};

	Cloner() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(Cloner::CHANNELS_PARAM, 1.0, 16.0, 1.0);
		configParam(Cloner::SPREAD_PARAM, 0.0, 2.0, 0.0);
		channels = 1;
	};

	void process(const ProcessArgs &args) override;

	int channels;
};


void Cloner::process(const ProcessArgs &args) {

	int channels_in = inputs[CHANNELS_INPUT].getChannels();
	float spread = params[SPREAD_PARAM].getValue();

	switch(channels_in) {
		case 0:  channels = params[CHANNELS_PARAM].getValue(); break;
		case 1:  channels = clamp(inputs[CHANNELS_INPUT].getVoltage()*2,1.0,16.0); break;
		default: channels = channels_in;
	}

	if (outputs[CV_OUTPUT].isConnected()) {

		float cv = inputs[CV_INPUT].getVoltageSum();
		outputs[CV_OUTPUT].setChannels(channels);
		for(int c=0; c<channels; c++) outputs[CV_OUTPUT].setVoltage(cv+c*spread, c);

	}
}


struct ChannelDisplayWidget : TransparentWidget {

  int *value=0;
  std::shared_ptr<Font> font;

  ChannelDisplayWidget() {
    font = APP->window->loadFont(asset::plugin(pluginInstance, "res/Segment7Standard.ttf"));
  };

  void draw(const DrawArgs &args) override {
    // Background
    NVGcolor backgroundColor = nvgRGB(0x20, 0x20, 0x20);
    NVGcolor borderColor = nvgRGB(0x10, 0x10, 0x10);
    nvgBeginPath(args.vg);
    nvgRoundedRect(args.vg, 0.0, 0.0, box.size.x, box.size.y, 4.0);
    nvgFillColor(args.vg, backgroundColor);
    nvgFill(args.vg);
    nvgStrokeWidth(args.vg, 1.0);
    nvgStrokeColor(args.vg, borderColor);
    nvgStroke(args.vg);

    nvgFontSize(args.vg, 18);
    nvgFontFaceId(args.vg, font->handle);
    nvgTextLetterSpacing(args.vg, 2.5);

    char displayStr[2];

    if(value) {sprintf(displayStr, "%2u", (unsigned) *value);}

    Vec textPos = Vec(6.0f, 17.0f);

    NVGcolor textColor = nvgRGB(0xdf, 0xd2, 0x2c);
    nvgFillColor(args.vg, nvgTransRGBA(textColor, 16));
    nvgText(args.vg, textPos.x, textPos.y, "~~", NULL);

    textColor = nvgRGB(0xda, 0xe9, 0x29);
    nvgFillColor(args.vg, nvgTransRGBA(textColor, 16));
    nvgText(args.vg, textPos.x, textPos.y, "\\\\", NULL);

    textColor = nvgRGB(0xf0, 0x00, 0x00);
    nvgFillColor(args.vg, textColor);
    nvgText(args.vg, textPos.x, textPos.y, displayStr,  NULL);
  }
};



struct ClonerWidget : ModuleWidget {
	ClonerWidget(Cloner *module);
};

ClonerWidget::ClonerWidget(Cloner *module) {

	setModule(module);
	box.size = Vec(15*3, 380);

	{
		SvgPanel *panel = new SvgPanel();
		panel->box.size = box.size;
		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance,"res/Cloner.svg")));
		addChild(panel);
	}

	addChild(createWidget<MLScrew>(Vec(15, 0)));
	addChild(createWidget<MLScrew>(Vec(15, 365)));

	ChannelDisplayWidget *display = new ChannelDisplayWidget();
	display->box.pos = Vec(4,40);
	display->box.size = Vec(37, 20);
	if(module) display->value = &(module->channels);
	addChild(display);


	addParam(createParam<SmallBlueSnapMLKnob>(Vec(9, 85), module, Cloner::CHANNELS_PARAM));
	addInput(createInput<MLPort>(Vec(9, 120), module, Cloner::CHANNELS_INPUT));

	addParam(createParam<SmallBlueMLKnob>(Vec(9, 175), module, Cloner::SPREAD_PARAM));
	addInput(createInput<MLPort>(Vec(9, 210), module, Cloner::SPREAD_INPUT));

	addInput(createInput<MLPort>(Vec(9, 246), module, Cloner::CV_INPUT));
	addOutput(createOutput<MLPort>(Vec(9, 292), module, Cloner::CV_OUTPUT));
}



Model *modelCloner = createModel<Cloner, ClonerWidget>("Cloner");
