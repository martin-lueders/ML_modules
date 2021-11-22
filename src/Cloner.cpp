#include "ML_modules.hpp"
#include "simd_mask.hpp"

using simd::float_4;


struct Cloner : Module {
	enum ParamIds {
		CHANNELS_PARAM,
		SPREAD_PARAM,
		CENTER_PARAM,
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
		configParam(Cloner::SPREAD_PARAM, 0.0, 1.0, 0.0);
		configParam(Cloner::CENTER_PARAM, 0.0, 2.0, 0.0);
		channels = 1;
		legacy = true;
	};

	void process(const ProcessArgs &args) override;

	int channels;
	bool legacy;


    json_t *dataToJson() override {
        json_t *rootJ = json_object();

        json_object_set_new(rootJ, "legacy", json_integer(legacy));

        return rootJ;
    }

    void dataFromJson(json_t *rootJ) override {

        json_t *legacyJ = json_object_get(rootJ, "legacy");
        if(legacyJ) legacy =  json_integer_value(legacyJ);

    }

};

void Cloner::process(const ProcessArgs &args) {

	int channels_in = inputs[CHANNELS_INPUT].getChannels();

	float spread = params[SPREAD_PARAM].getValue() * inputs[SPREAD_INPUT].getNormalVoltage(2.0);



	switch(channels_in) {
		case 0:  channels = params[CHANNELS_PARAM].getValue(); break;
		case 1:  channels = clamp(inputs[CHANNELS_INPUT].getVoltage()*2,1.0,16.0); break;
		default: channels = channels_in;
	}

	float offset = params[CENTER_PARAM].getValue() > 0.5 ? (channels-1)/2.0 : 0;

	float div = legacy ? 1.0 : MAX(1,channels-1);

	if (outputs[CV_OUTPUT].isConnected()) {

		float cv = inputs[CV_INPUT].getVoltageSum();
		outputs[CV_OUTPUT].setChannels(channels);
		for(int c=0; c<channels; c++) outputs[CV_OUTPUT].setVoltage(clamp(cv + (c-offset)*spread/(div), -10.0f, 10.0f), c);

	}
}


struct ClonerModeItem : MenuItem {

        Cloner *cloner;

 
        void onAction(const event::Action &e) override {
                cloner->legacy = !cloner->legacy;
        };

        void step() override {
                rightText = (cloner->legacy)? "✔" : "";
        };

};


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

    char displayStr[3];

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
	json_t *dataToJsonData() ;
	void dataFromJsonData(json_t *root) ;
	void appendContextMenu(Menu *) override;
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


	addParam(createParam<SmallBlueSnapMLKnob>(Vec(9, 75), module, Cloner::CHANNELS_PARAM));
	addInput(createInput<MLPort>(Vec(9, 110), module, Cloner::CHANNELS_INPUT));

	addParam(createParam<SmallBlueMLKnob>(Vec(9, 155), module, Cloner::SPREAD_PARAM));
	addInput(createInput<MLPort>(Vec(9, 190), module, Cloner::SPREAD_INPUT));

    addParam(createParam<CKSS>(   Vec(9, 225), module, Cloner::CENTER_PARAM));


	addInput(createInput<MLPort>(Vec(9, 264), module, Cloner::CV_INPUT));
	addOutput(createOutput<MLPort>(Vec(9, 307), module, Cloner::CV_OUTPUT));


}

void ClonerWidget::appendContextMenu(Menu *menu) {

    Cloner *cloner = dynamic_cast<Cloner*>(module);
	assert(cloner);
	
    MenuLabel *modeLabel = new MenuLabel();
    modeLabel->text = "Cloner Mode";
    menu->addChild(modeLabel);

    ClonerModeItem *legacy_Item = new ClonerModeItem();
    legacy_Item->text = "Legacy";
    legacy_Item->cloner = cloner;
    menu->addChild(legacy_Item);


}

Model *modelCloner = createModel<Cloner, ClonerWidget>("Cloner");
