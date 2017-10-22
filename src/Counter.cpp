#include "ML_modules.hpp"
#include "dsp/digital.hpp"

#include <sstream>
#include <iomanip>

struct Counter : Module {
	enum ParamIds {
		MAX_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		LENGTH_INPUT,
		GATE_INPUT,
		START_INPUT,
		STOP_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		GATE_OUTPUT,
		START_OUTPUT,
		STOP_OUTPUT,
		NUM_OUTPUTS
	};

	Counter() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS) {};

	void step() override;

#ifdef v040
	void initialize() override {counter=0; state=false; state2=false;};
#endif
#ifdef v_dev
	void reset() override {counter=0; state=false; state2=false; gSampleRate=engineGetSampleRate();};
#endif

	int counter = 0;
	bool state = false;
	bool state2 = false;
	float stateLight;

#ifdef v_dev	
	float gSampleRate;

	void onSampleRateChange() override {gSampleRate = engineGetSampleRate();}
#endif

        int max;
  
	SchmittTrigger startTrigger, gateTrigger, stopTrigger;
	PulseGenerator startPulse, stopPulse;
};



void Counter::step() {


	max = params[MAX_PARAM].value;


	if( inputs[LENGTH_INPUT].active ) max = max * clampf(inputs[LENGTH_INPUT].value/10.0,0,1.0);

	if( startTrigger.process(inputs[START_INPUT].normalize(0.0) )) {
		state=true; 
		counter=gateTrigger.isHigh()?1:0;
		startPulse.trigger(0.001);
	};

	if( stopTrigger.process(inputs[STOP_INPUT].normalize(0.0) ))   {
		state=false; 
		counter=0;
	};

	if( gateTrigger.process(inputs[GATE_INPUT].normalize(0.0) ) ) {

		if(state) counter++; 

		if(counter > max) {
			
			counter = 0;
			state   = false;
			stopPulse.trigger(0.001);
		};
		
	};


	float out = (gateTrigger.isHigh()&&state) ? 10.0 : 0.0;

	float start = (startPulse.process(1.0/gSampleRate)) ? 10.0 : 0.0;
	float stop  = (stopPulse.process(1.0/gSampleRate)) ? 10.0 : 0.0;

	outputs[GATE_OUTPUT].value = out;
	outputs[START_OUTPUT].value = start;
	outputs[STOP_OUTPUT].value = stop;

};

struct NumberDisplayWidget : TransparentWidget {

  int *value;
  std::shared_ptr<Font> font;

  NumberDisplayWidget() {
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

//    std::string to_display = std::to_string(*value);
    std::stringstream to_display;
   
    to_display << std::setw(3) << *value;

    Vec textPos = Vec(6.0f, 17.0f);

    NVGcolor textColor = nvgRGB(0xdf, 0xd2, 0x2c);
    nvgFillColor(vg, nvgTransRGBA(textColor, 16));
    nvgText(vg, textPos.x, textPos.y, "~~~", NULL);

    textColor = nvgRGB(0xda, 0xe9, 0x29);
    nvgFillColor(vg, nvgTransRGBA(textColor, 16));
    nvgText(vg, textPos.x, textPos.y, "\\\\\\", NULL);

    textColor = nvgRGB(0xf0, 0x00, 0x00);
    nvgFillColor(vg, textColor);
    nvgText(vg, textPos.x, textPos.y, to_display.str().c_str(), NULL);
  }
};




  
  


CounterWidget::CounterWidget() {
	Counter *module = new Counter();
	setModule(module);
	box.size = Vec(15*6, 380);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin,"res/Counter.svg")));
		addChild(panel);
	}


	

	addChild(createScrew<ScrewSilver>(Vec(15, 0)));
	addChild(createScrew<ScrewSilver>(Vec(15, 365)));

	addParam(createParam<Davies1900hSmallBlackKnob>(Vec(12,  85), module, Counter::MAX_PARAM, 0.0, 128.0, 8.0));
	addInput(createInput<PJ301MPort>(Vec(53, 87), module, Counter::LENGTH_INPUT));

	addInput(createInput<PJ301MPort>(Vec(13, 168), module, Counter::GATE_INPUT));
	addOutput(createOutput<PJ301MPort>(Vec(53, 168), module, Counter::GATE_OUTPUT));


	addInput(createInput<PJ301MPort>(Vec(13, 235), module, Counter::START_INPUT));
	addOutput(createOutput<PJ301MPort>(Vec(53, 235), module, Counter::START_OUTPUT));

	addInput(createInput<PJ301MPort>(Vec(13, 298), module, Counter::STOP_INPUT));
	addOutput(createOutput<PJ301MPort>(Vec(53, 298), module, Counter::STOP_OUTPUT));

	NumberDisplayWidget *display = new NumberDisplayWidget();
	display->box.pos = Vec(20,56);
	display->box.size = Vec(50, 20);
	display->value = &module->max;
	addChild(display);

	
	NumberDisplayWidget *display2 = new NumberDisplayWidget();
	display2->box.pos = Vec(20,145);
	display2->box.size = Vec(50, 20);
	display2->value = &module->counter;
	addChild(display2);

}
