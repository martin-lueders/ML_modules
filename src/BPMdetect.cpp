#include "ML_modules.hpp"
// #include "math.hpp"

#include <iostream>
#include <sstream>
#include <iomanip>


struct BPMdetect : Module {
	enum ParamIds {
		SMOOTH_PARAM,
		MULT2_PARAM,
		MULT3_PARAM,
		SWING2_PARAM,
		SWING3_PARAM,
		DELAY1_PARAM,
		DELAY2_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		GATE_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		LFO_OUTPUT,
		SEQ_OUTPUT,
		DELAY_OUTPUT,
		TRIG1_OUTPUT,
		TRIG2_OUTPUT,
		TRIG3_OUTPUT,
		NUM_OUTPUTS
	};
	enum LighIds {
		NUM_LIGHTS
	};

	BPMdetect() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS); misses = 0; 
	
	    configParam(BPMdetect::SMOOTH_PARAM, 0.0, 1.0, 0.5);
        configParam(BPMdetect::MULT2_PARAM, 1.0, 8.0, 2.0);
        configParam(BPMdetect::SWING2_PARAM, 0.0, 2.0, 1.0);
        configParam(BPMdetect::MULT3_PARAM, 1.0, 8.0, 3.0);
        configParam(BPMdetect::SWING3_PARAM, 0.0, 2.0, 1.0);
        configParam(BPMdetect::DELAY1_PARAM, 1.0, 8.0, 1.0);
        configParam(BPMdetect::DELAY2_PARAM, 1.0, 8.0, 1.0);

	};

	void process(const ProcessArgs &args) override;

	int misses = 0;
	int count2 = 0;
	int count3 = 0;

	float timer = 0.0;
	float timer1 = 0.0;
	float timer2 = 0.0;
	float timer3 = 0.0;
	float seconds = 0.0;
	float BPM=0.0;
	float lfo_volts=0.0;
	float delay_volts=0.0;

	bool fine = false;

	inline bool checkBeat(float timer, int mult) {
		return ( ((timer - mult*seconds) * (timer - mult*seconds) / (seconds*seconds) < 0.2 ) && misses < 4);
	}

	// float gSampleRate;
	// void reset() {onSampleRateChange();};
	// void onSampleRateChange() override {gSampleRate = args.sampleRate; deltaT = 1.0/gSampleRate;}

	dsp::SchmittTrigger gateTrigger;
	dsp::PulseGenerator outPulse1, outPulse2, outPulse3;
};



void BPMdetect::process(const ProcessArgs &args) {

	float deltaT=args.sampleTime;

	float mult2 = roundf(params[MULT2_PARAM].getValue());
	float mult3 = roundf(params[MULT3_PARAM].getValue());

	float factor2 = ( fine ? 1.0f + 0.25f * (params[SWING2_PARAM].getValue() - 1.0f): params[SWING2_PARAM].getValue() ) / mult2;
	float factor3 = ( fine ? 1.0f + 0.25f * (params[SWING3_PARAM].getValue() - 1.0f): params[SWING3_PARAM].getValue() ) / mult3;


	if( inputs[GATE_INPUT].isConnected()) {

		if( timer1 > seconds ) {
			outPulse1.trigger(0.01);
			timer1 = 0.0;
		}

		if( (timer2 > seconds*factor2) /* && (count2 < mult2) */ ) {
//			if(nearf(factor2,1.0)) std::cerr << timer2 << "\n";
			outPulse2.trigger(0.01);
			timer2 = 0.0;
			// count2++;
		}

		if( (timer3 > seconds*factor3) /* && (count3<mult3) */ ) {
			outPulse3.trigger(0.01);
			timer3 = 0.0 ;
			// count3++;
		}

		if( gateTrigger.process(inputs[GATE_INPUT].getVoltage()) ) {


			if(timer>0) {
				float new_seconds;


				bool found=false;

				for(int mult=1;  !found && mult < 20; mult++ )  {
					if(checkBeat(timer, mult)) {
						new_seconds = timer/mult;
						if(mult==1) misses=0;
						else        misses++;
						found = true;
					};
				};

				if( !found ) {
//					std::cerr << "default. misses = " << misses << "\n";
					new_seconds = timer;
					misses=0;
				}


				float a = params[SMOOTH_PARAM].getValue();
				seconds = ( (1.0-a)*seconds + a*new_seconds);
				BPM=60.0/seconds;

				lfo_volts = 1.0 - log2(seconds) ;

				float num   = roundf(params[DELAY1_PARAM].getValue());
				float denom = roundf(params[DELAY2_PARAM].getValue());

				delay_volts = 10.0*(3.0+log10(seconds * num/denom))/4.0;

				timer -= seconds;
				timer1 = 0.0;
				timer2 = 0.0;
				timer3 = 0.0;
				count2 = 1;
				count3 = 1;
				outPulse1.trigger(0.01);
				outPulse2.trigger(0.01);
				outPulse3.trigger(0.01);

			}

		};

	};

	timer += deltaT;
	timer1 += deltaT;
	timer2 += deltaT;
	timer3 += deltaT;

	outputs[TRIG1_OUTPUT].setVoltage(outPulse1.process(deltaT) ? 10.0 : 0.0);
	outputs[TRIG2_OUTPUT].setVoltage(outPulse2.process(deltaT) ? 10.0 : 0.0);
	outputs[TRIG3_OUTPUT].setVoltage(outPulse3.process(deltaT) ? 10.0 : 0.0);


	outputs[LFO_OUTPUT].setVoltage(lfo_volts);
	outputs[SEQ_OUTPUT].setVoltage(lfo_volts-3.0);
	outputs[DELAY_OUTPUT].setVoltage(delay_volts);

};

struct NumberDisplayWidget2 : TransparentWidget {

  float  *value;
  std::shared_ptr<Font> font;

  NumberDisplayWidget2() {
    font = APP->window->loadFont(asset::plugin(pluginInstance, "res/Segment7Standard.ttf"));
  };

  void draw(const DrawArgs &args) {
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

    char display_string[10];

    sprintf(display_string,"%6.1f",*value);

    Vec textPos = Vec(6.0f, 17.0f);

    NVGcolor textColor = nvgRGB(0xdf, 0xd2, 0x2c);
    nvgFillColor(args.vg, nvgTransRGBA(textColor, 16));
    nvgText(args.vg, textPos.x, textPos.y, "~~~~~", NULL);

    textColor = nvgRGB(0xda, 0xe9, 0x29);
    nvgFillColor(args.vg, nvgTransRGBA(textColor, 16));
    nvgText(args.vg, textPos.x, textPos.y, "\\\\\\\\\\", NULL);

    textColor = nvgRGB(0xf0, 0x00, 0x00);
    nvgFillColor(args.vg, textColor);
    nvgText(args.vg, textPos.x, textPos.y, display_string, NULL);
  }
};



struct BPMdetectWidget : ModuleWidget {

	json_t *dataToJsonData() ;
	void dataFromJsonData(json_t *root);



	BPMdetectWidget(BPMdetect *module) {
		setModule(module);
		box.size = Vec(15*10, 380);

		{
			SvgPanel *panel = new SvgPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance,"res/BPMdetect.svg")));
			addChild(panel);
		}

		const float column1 = 15;
		const float column2 = 61;
		const float column3 = 110;

		const float row1 = 84;
		const float row2 = 140;
		const float row3 = row2 + 60;
		const float row4 = row3 + 58;
		const float row5 = 316;

		addChild(createWidget<MLScrew>(Vec(15, 0)));
		addChild(createWidget<MLScrew>(Vec(box.size.x-30, 0)));
		addChild(createWidget<MLScrew>(Vec(15, 365)));
		addChild(createWidget<MLScrew>(Vec(box.size.x-30, 365)));

		addInput(createInput<MLPort>(Vec(column1+5,  row1+2), module, BPMdetect::GATE_INPUT));
  	addParam(createParam<SmallBlueMLKnob>(Vec(column2,   row1), module, BPMdetect::SMOOTH_PARAM));
		addOutput(createOutput<MLPort>(Vec(column3-5,row1+2), module, BPMdetect::TRIG1_OUTPUT));

	  addParam(createParam<SmallBlueSnapMLKnob>(Vec(column1,  row2),    module, BPMdetect::MULT2_PARAM));
  	addParam(createParam<SmallBlueMLKnob>(Vec(column2,  row2),    module, BPMdetect::SWING2_PARAM));
		addOutput(createOutput<MLPort>(Vec(column3, row2+2), module, BPMdetect::TRIG2_OUTPUT));

	  addParam(createParam<SmallBlueSnapMLKnob>(Vec(column1,  row3),    module, BPMdetect::MULT3_PARAM));
  	addParam(createParam<SmallBlueMLKnob>(Vec(column2,  row3),    module, BPMdetect::SWING3_PARAM));
		addOutput(createOutput<MLPort>(Vec(column3, row3+2), module, BPMdetect::TRIG3_OUTPUT));

		addOutput(createOutput<MLPort>(Vec(column1, row4), module, BPMdetect::LFO_OUTPUT));
		addOutput(createOutput<MLPort>(Vec(column3, row4), module, BPMdetect::SEQ_OUTPUT));

  	addParam(createParam<SmallBlueSnapMLKnob>(Vec(column1,  row5), module, BPMdetect::DELAY1_PARAM));
  	addParam(createParam<SmallBlueSnapMLKnob>(Vec(column2,  row5), module, BPMdetect::DELAY2_PARAM));
		addOutput(createOutput<MLPort>(Vec(column3, row5), module, BPMdetect::DELAY_OUTPUT));

		NumberDisplayWidget2 *display = new NumberDisplayWidget2();
		display->box.pos = Vec(25,40);
		display->box.size = Vec(100, 20);
		if(module) display->value = &module->BPM;
		addChild(display);
	};

	void appendContextMenu(Menu *menu) override {

	    BPMdetect *myModule = dynamic_cast<BPMdetect*>(module);
		assert(myModule);

		struct FineMenuItem : MenuItem {

    		BPMdetect *module;
    		bool mfine;

    		void onAction(const event::Action &e) override {
    		    module->fine = mfine;
    		};

    		void step() override {
        	    rightText = (module->fine == mfine)? "✔" : "";
        	};

		};

		struct NormalMenuItem : MenuItem {

	    	BPMdetect *module;
	    	bool mfine;

    		void onAction(const event::Action &e) override {
        	    module->fine = mfine;
    		};

	    	void step() override {
	    	    rightText = (module->fine != mfine)? "✔" : "";
	       	};

		};
    
	    menu->addChild(construct<MenuLabel>());

    	menu->addChild(construct<MenuLabel>(&MenuLabel::text, "Swing Range"));
    	menu->addChild(construct<FineMenuItem>(&MenuItem::text, "Fine", &FineMenuItem::module, myModule, &FineMenuItem::mfine, true));
    	menu->addChild(construct<NormalMenuItem>(&MenuItem::text, "Legacy", &NormalMenuItem::module, myModule, &NormalMenuItem::mfine, false));
	};
};



Model *modelBPMdetect = createModel<BPMdetect, BPMdetectWidget>("BPMdetect");
