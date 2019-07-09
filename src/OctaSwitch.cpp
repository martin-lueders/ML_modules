#include "ML_modules.hpp"

using simd::float_4;

struct OctaSwitch : Module {
	enum ParamIds {
		THRESHOLD_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		THRESHOLD_INPUT,
		GATE_INPUT,
		A_INPUT    = GATE_INPUT + 8,
		B_INPUT    = A_INPUT + 8,
		NUM_INPUTS = B_INPUT + 8
	};
	enum OutputIds {
		OUT_OUTPUT,
		NUM_OUTPUTS = OUT_OUTPUT + 8
	};

	enum LightIds {
		NUM_LIGHTS
	};

	float threshold = 0.0;

	OctaSwitch() {
		config( NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS ); 
		configParam(OctaSwitch::THRESHOLD_PARAM, -5.0, 10.0, 1.0);
 
	};

	void process(const ProcessArgs &args) override;

};



void OctaSwitch::process(const ProcessArgs &args) {

	threshold = inputs[THRESHOLD_INPUT].getNormalVoltage(params[THRESHOLD_PARAM].getValue());
	float_4 threshold_4 = float_4(threshold);

	for(int i=0; i<8; i++) {
		
		int gate_channels = inputs[GATE_INPUT].getChannels();
		int in_A_channels = inputs[A_INPUT+i].getChannels();
		int in_B_channels = inputs[B_INPUT+i].getChannels();
		
		int in_channels = MAX(in_A_channels, in_B_channels);
		int channels = MAX(gate_channels, in_channels);

		for(int c=0; c<channels; c+=4) {

			simd::float_4 gates = inputs[GATE_INPUT+i].getPolyVoltageSimd<float_4>(c);
			simd::float_4 in_A  = inputs[A_INPUT+i].getPolyVoltageSimd<float_4>(c);
			simd::float_4 in_B  = inputs[B_INPUT+i].getPolyVoltageSimd<float_4>(c);

			simd::float_4 out = ifelse( gates > threshold_4, in_B, in_A);

			out.store(outputs[OUT_OUTPUT+i].getVoltages(c));

		}
	}
};

struct ThresholdDisplayWidget : TransparentWidget {

  float  *value=0 ;

  std::shared_ptr<Font> font;

  ThresholdDisplayWidget() {
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
    nvgTextLetterSpacing(args.vg, 1.0);

    char display_string[10];

    if(value) sprintf(display_string,"%5.1f",*value);

    Vec textPos = Vec(3.0f, 17.0f);

    NVGcolor textColor = nvgRGB(0xdf, 0xd2, 0x2c);
    nvgFillColor(args.vg, nvgTransRGBA(textColor, 16));
    nvgText(args.vg, textPos.x, textPos.y, "~~~~", NULL);

    textColor = nvgRGB(0xda, 0xe9, 0x29);
    nvgFillColor(args.vg, nvgTransRGBA(textColor, 16));
    nvgText(args.vg, textPos.x, textPos.y, "\\\\\\\\", NULL);

	{
	    textColor = nvgRGB(0xf0, 0x00, 0x00);
		nvgFillColor(args.vg, textColor);
		nvgText(args.vg, textPos.x, textPos.y, display_string, NULL);
	};
  }
};

struct OctaSwitchWidget : ModuleWidget {
	OctaSwitchWidget(OctaSwitch *module);
};

OctaSwitchWidget::OctaSwitchWidget(OctaSwitch *module) {
		setModule(module);

	box.size = Vec(15*10, 380);

	{
		SvgPanel *panel = new SvgPanel();
		panel->box.size = box.size;
		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance,"res/OctaSwitch.svg")));

		addChild(panel);
	}

	addChild(createWidget<MLScrew>(Vec(15, 0)));
	addChild(createWidget<MLScrew>(Vec(box.size.x-30, 0)));
	addChild(createWidget<MLScrew>(Vec(15, 365)));
	addChild(createWidget<MLScrew>(Vec(box.size.x-30, 365)));



	const float offset_y = 60, delta_y = 32, row1=15, row2 = 47, row3 = 77, row4 = 110;

	addInput(createInput<MLPort>(   Vec(row1,  328 ), module, OctaSwitch::THRESHOLD_INPUT));
	addParam(createParam<SmallBlueMLKnob>(  Vec(row2-5,  326), module, OctaSwitch::THRESHOLD_PARAM));

	for( int i=0; i<8; i++) {
		addInput(createInput<MLPort>(Vec(row1, offset_y + i*delta_y ), module, OctaSwitch::GATE_INPUT+i));
		addInput(createInput<MLPort>(Vec(row2, offset_y + i*delta_y ), module, OctaSwitch::A_INPUT+i));
		addInput(createInput<MLPort>(Vec(row3, offset_y + i*delta_y ), module, OctaSwitch::B_INPUT+i));

		addOutput(createOutput<MLPort>(Vec(row4, offset_y + i*delta_y ), module, OctaSwitch::OUT_OUTPUT+i));
	};

	ThresholdDisplayWidget *display = new ThresholdDisplayWidget();
	display->box.pos = Vec(row3-3,330);
	display->box.size = Vec(65, 20);
	if(module) display->value = &module->threshold;
	addChild(display);

}


Model *modelOctaSwitch = createModel<OctaSwitch, OctaSwitchWidget>("OctaSwitch");
