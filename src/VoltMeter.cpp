#include "ML_modules.hpp"

#include <sstream>
#include <iomanip>

struct VoltMeter : Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		IN1_INPUT,
		IN2_INPUT,
		IN3_INPUT,
		IN4_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};


	VoltMeter() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS); for(int i=0; i<4; i++) {volts[i] = 0.0f; active[i] = false;}};


	void process(const ProcessArgs &args) override;

	float volts[4];
	bool active[4];

};



void VoltMeter::process(const ProcessArgs &args) {

	for(int i=0; i<4; i++) {
		active[i] = inputs[IN1_INPUT+i].isConnected();
		volts[i] = 0.9 * volts[i] + 0.1 * inputs[IN1_INPUT+i].getNormalVoltage(0.0);
	};

};

struct VoltDisplayWidget : TransparentWidget {

  float  *value=0;
  bool *on = 0;

  std::shared_ptr<Font> font;

  VoltDisplayWidget() {
    font = APP->window->loadFont(asset::plugin(pluginInstance, "res/Segment7Standard.ttf"));
  };

  void draw(const DrawArgs &args) {
    // Background
//    NVGcolor backgroundColor = nvgRGB(0x44, 0x44, 0x44);
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

    char display_string[10];

    if(value) sprintf(display_string,"%7.3f",*value);

    Vec textPos = Vec(6.0f, 17.0f);

    NVGcolor textColor = nvgRGB(0xdf, 0xd2, 0x2c);
    nvgFillColor(args.vg, nvgTransRGBA(textColor, 16));
    nvgText(args.vg, textPos.x, textPos.y, "~~~~~~", NULL);

    textColor = nvgRGB(0xda, 0xe9, 0x29);
    nvgFillColor(args.vg, nvgTransRGBA(textColor, 16));
    nvgText(args.vg, textPos.x, textPos.y, "\\\\\\\\\\\\\\", NULL);

	if(on && *on) {
	    textColor = nvgRGB(0xf0, 0x00, 0x00);
		nvgFillColor(args.vg, textColor);
		nvgText(args.vg, textPos.x, textPos.y, display_string, NULL);
	};
  }
};


struct VoltMeterWidget : ModuleWidget {
	VoltMeterWidget(VoltMeter *module);
	TextField ** label;
};

VoltMeterWidget::VoltMeterWidget(VoltMeter *module) {
		setModule(module);

	box.size = Vec(15*8, 380);

//	label = new TextField*[4];

	{
		SvgPanel *panel = new SvgPanel();
		panel->box.size = box.size;
		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance,"res/VoltMeter.svg")));
		addChild(panel);
	}


	const float delta_y = 70;

	addChild(createWidget<MLScrew>(Vec(15, 0)));
	addChild(createWidget<MLScrew>(Vec(15, 365)));
	addChild(createWidget<MLScrew>(Vec(box.size.x-30, 0)));
	addChild(createWidget<MLScrew>(Vec(box.size.x-30, 365)));


	for(int i=0; i<4; i++) {

		addInput(createInput<MLPort>(Vec(12, 60+i*delta_y), module, VoltMeter::IN1_INPUT+i));


		VoltDisplayWidget *display = new VoltDisplayWidget();
		display->box.pos = Vec(10,90+i*delta_y);
		display->box.size = Vec(100, 20);
		if(module) display->value = &module->volts[i];
		if(module) display->on = &module->active[i];
		addChild(display);

//		label[i] = new TextField();
//		label[i]->box.pos = Vec(50,60+i*65);
//		label[i]->box.size = Vec(60,30);
//		addChild(label[i]);
	};
	

}

Model *modelVoltMeter = createModel<VoltMeter, VoltMeterWidget>("VoltMeter");
