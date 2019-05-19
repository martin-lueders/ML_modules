#include "ML_modules.hpp"


struct TrigBuf : Module {
	enum ParamIds {
		ARM1_PARAM,
		ARM2_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		ARM1_INPUT,
		ARM2_INPUT,
		GATE1_INPUT,
		GATE2_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		ARM1_LIGHT,
		ARM2_LIGHT,
		NUM_LIGHTS
	};


	TrigBuf() : Module( NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS ) { reset(); };


	void process(const ProcessArgs &args) override;

	float arm1=0.0, arm2=0.0;
	float out1=0.0, out2=0.0;

	bool gate1=false, gate2=false;
	bool delayed1=false, delayed2=false;

	dsp::SchmittTrigger armTrigger1, armTrigger2;
	dsp::SchmittTrigger gateTrigger1, gateTrigger2;

	void reset() override {

		arm1=0.0;
		arm2=0.0;
		out1=0.0;
		out2=0.0;
		gate1=false;
		gate2=false;
		delayed1=false;
		delayed2=false;
	};

private:

	bool neg_slope(bool gate, bool last_gate) { return (gate!=last_gate) && last_gate; }


};




void TrigBuf::process(const ProcessArgs &args) {


	bool last_gate1 = gate1;
	bool last_gate2 = gate2;
       

	gateTrigger1.process(inputs[GATE1_INPUT].normalize(0.0f));
	gate1 = (gateTrigger1.isHigh());

	gateTrigger2.process(inputs[GATE2_INPUT].normalize(inputs[GATE1_INPUT].normalize(0.0f)));
	gate2 = gateTrigger2.isHigh();
	

	
	if( armTrigger1.process(inputs[ARM1_INPUT].normalize(0.0f) + params[ARM1_PARAM].getValue() ) ) { 
		if (!gate1) {arm1 = 10.0;}
		else { 
				// arm1 = 0.0;
			delayed1 = true;
		};
	}
	

    if(gate1) {
		if(arm1 > 5.0) {
			out1 = 10.0;
		} else {
			out1 = 0.0;
		};
	}
	else {
		if(out1 > 5.0) {
			arm1 = 0.0;
			out1 = 0.0;
		};
	};

	if( delayed1 && neg_slope(gate1, last_gate1) ) {

		arm1 = 10.0;
		delayed1 = false;

	};



	if( armTrigger2.process(inputs[ARM2_INPUT].normalize(inputs[ARM1_INPUT].normalize(0.0f)) + params[ARM2_PARAM].getValue() ) ) { 
		if (!gate2) {arm2 = 10.0;}
		else { 
			// arm2 = 0.0;
			delayed2 = true;
		};
	};
	

	if (gate2) {

		if(arm2 > 5.0) out2 = 10.0;
		else {
			out2 = 0.0;
		};
	} else {
		if(out2 > 5.0) {
			arm2 = 0.0;
			out2 = 0.0;
		};
	};

	if( delayed2 && neg_slope(gate2, last_gate2) ) {

		arm2 = 10.0;
		delayed2 = false;

	};


	outputs[OUT1_OUTPUT].setVoltage(out1);
	outputs[OUT2_OUTPUT].setVoltage(out2);

	lights[ARM1_LIGHT].value = arm1;
	lights[ARM2_LIGHT].value = arm2;
};



struct TrigBufWidget : ModuleWidget {
	TrigBufWidget(TrigBuf *module);
};

TrigBufWidget::TrigBufWidget(TrigBuf *module) {
		setModule(module);

	box.size = Vec(15*4, 380);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance,"res/TrigBuf.svg")));
		addChild(panel);
	}

	addChild(createWidget<MLScrew>(Vec(15, 0)));
	addChild(createWidget<MLScrew>(Vec(15, 365)));

	addInput(createInput<MLPort>(Vec(9, 62), module, TrigBuf::ARM1_INPUT));
	addInput(createInput<MLPort>(Vec(9, 105), module, TrigBuf::GATE1_INPUT));
	addOutput(createOutput<MLPort>(Vec(9, 150), module, TrigBuf::OUT1_OUTPUT));

	addParam(createParam<ML_SmallLEDButton>(Vec(40,66), module, TrigBuf::ARM1_PARAM, 0, 10, 0));
	addChild(createLight<MLSmallLight<GreenLight>>(Vec(44, 70), module, TrigBuf::ARM1_LIGHT));

	addInput(createInput<MLPort>(Vec(9, 218), module, TrigBuf::ARM2_INPUT));
	addInput(createInput<MLPort>(Vec(9, 263), module, TrigBuf::GATE2_INPUT));
	addOutput(createOutput<MLPort>(Vec(9, 305), module, TrigBuf::OUT2_OUTPUT));

	addParam(createParam<ML_SmallLEDButton>(Vec(40,222), module, TrigBuf::ARM2_PARAM, 0, 10, 0));
	addChild(createLight<MLSmallLight<GreenLight>>(Vec(44, 226), module, TrigBuf::ARM2_LIGHT));
}

Model *modelTrigBuf = createModel<TrigBuf, TrigBufWidget>("TrigBuf");
