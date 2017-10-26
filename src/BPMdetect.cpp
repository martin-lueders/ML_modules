#include "ML_modules.hpp"
#include "dsp/digital.hpp"

#include <sstream>
#include <iomanip>

struct BPMdetect : Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		GATE_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		LFO_OUTPUT,
		DELAY_OUTPUT,
		NUM_OUTPUTS
	};

	BPMdetect() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS) { onSampleRateChange();};

	void step() override;

	int counter = 0;
	float timer = 0.0;
	float seconds = 0.0;
	float deltaT;
	float BPM=0.0;
	float lfo_volts=0.0;
	float delay_volts=0.0;


#ifdef v040
	void initialize() override {counter=0; onSampleRateChange(); };
	void onSampleRateChange() override {deltaT = 1.0/gSampleRate;}
#endif

#ifdef v_dev
	float gSampleRate;
	void reset() override {counter=0; onSampleRateChange();};
	void onSampleRateChange() override {gSampleRate = engineGetSampleRate(); deltaT = 1.0/gSampleRate;}
#endif

	SchmittTrigger gateTrigger;
};



void BPMdetect::step() {

	if( inputs[GATE_INPUT].active) {

		if( gateTrigger.process(inputs[GATE_INPUT].value) ) {

			seconds = timer;
			if(timer>0) {
				seconds = (3*seconds + timer)/4.0;
				BPM=60.0/seconds;

				lfo_volts = 1.0 - log2(seconds) ;
				delay_volts = 10.0*(3.0+log10(seconds))/4.0;
			}
			timer = 0.0;


		};

	};
	
	timer += deltaT;

	outputs[LFO_OUTPUT].value = lfo_volts;
	outputs[DELAY_OUTPUT].value = delay_volts;

};

struct NumberDisplayWidget2 : TransparentWidget {

  float  *value;
  std::shared_ptr<Font> font;

  NumberDisplayWidget2() {
    font = Font::load(assetPlugin(plugin, "res/Segment7Standard.ttf"));
  };

  void draw(NVGcontext *vg) {
    // Background
    NVGcolor backgroundColor = nvgRGB(0x44, 0x44, 0x44);
    NVGcolor borderColor = nvgRGB(0x10, 0x10, 0x10);
    nvgBeginPath(vg);
    nvgRoundedRect(vg, 0.0, 0.0, box.size.x, box.size.y, 4.0);
    nvgFillColor(vg, backgroundColor);
    nvgFill(vg);
    nvgStrokeWidth(vg, 1.0);
    nvgStrokeColor(vg, borderColor);
    nvgStroke(vg);

    nvgFontSize(vg, 18);
    nvgFontFaceId(vg, font->handle);
    nvgTextLetterSpacing(vg, 2.5);

    char display_string[10];
//    std::string to_display = std::to_string(*value);
//    std::stringstream to_display;
   
//    to_display << std::setw(7) << std::setprecision(2) << *value;

    sprintf(display_string,"%6.1f",*value);

    Vec textPos = Vec(6.0f, 17.0f);

    NVGcolor textColor = nvgRGB(0xdf, 0xd2, 0x2c);
    nvgFillColor(vg, nvgTransRGBA(textColor, 16));
    nvgText(vg, textPos.x, textPos.y, "~~~", NULL);

    textColor = nvgRGB(0xda, 0xe9, 0x29);
    nvgFillColor(vg, nvgTransRGBA(textColor, 16));
    nvgText(vg, textPos.x, textPos.y, "\\\\\\", NULL);

    textColor = nvgRGB(0xf0, 0x00, 0x00);
    nvgFillColor(vg, textColor);
 //   nvgText(vg, textPos.x, textPos.y, to_display.str().c_str(), NULL);
 //   nvgText(vg, textPos.x, textPos.y, to_display.c_str(), NULL);
    nvgText(vg, textPos.x, textPos.y, display_string, NULL);
  }
};


BPMdetectWidget::BPMdetectWidget() {
	BPMdetect *module = new BPMdetect();
	setModule(module);
	box.size = Vec(15*10, 380);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin,"res/BPMdetect.svg")));
		addChild(panel);
	}


	

	addChild(createScrew<ScrewSilver>(Vec(15, 0)));
	addChild(createScrew<ScrewSilver>(Vec(15, 365)));


	addInput(createInput<PJ301MPort>(Vec(13, 150), module, BPMdetect::GATE_INPUT));
	addOutput(createOutput<PJ301MPort>(Vec(53, 150), module, BPMdetect::LFO_OUTPUT));
	addOutput(createOutput<PJ301MPort>(Vec(53, 200), module, BPMdetect::DELAY_OUTPUT));


	NumberDisplayWidget2 *display = new NumberDisplayWidget2();
	display->box.pos = Vec(20,56);
	display->box.size = Vec(100, 20);
	display->value = &module->BPM;
	addChild(display);

	

}
