#include "ML_modules.hpp"



#define minLength 0.001f

struct TrigDelay : Module {
	enum ParamIds {
		DELAY1_PARAM,
		DELAY2_PARAM,
		LENGTH1_PARAM,
		LENGTH2_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		GATE1_INPUT,
		GATE2_INPUT,
		DELAY1_INPUT,
		DELAY2_INPUT,
		LENGTH1_INPUT,
		LENGTH2_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};


	TrigDelay() : Module( NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS ) {

		gSampleRate = engineGetSampleRate();
		// minLength = 0.001;

	};


	void step() override;

	bool gate1=false, gate2=false;

	SchmittTrigger gateTrigger1, gateTrigger2;
	PulseGenerator delay1,  delay2;
	PulseGenerator on1, on2;



	float gSampleRate;

	void onSampleRateChange() { gSampleRate = engineGetSampleRate(); }

	void reset() override {

		gate1=false;
		gate2=false;
	};

private:



};




void TrigDelay::step() {


	float delayTime1 = params[DELAY1_PARAM].value;
	float delayTime2 = params[DELAY2_PARAM].value;
	float length1    = params[LENGTH1_PARAM].value;
	float length2    = params[LENGTH2_PARAM].value;


	if( inputs[DELAY1_INPUT].active )  { delayTime1 *= clamp( inputs[DELAY1_INPUT].value / 10.0f, 0.0f, 1.0f );};
	if( inputs[DELAY2_INPUT].active )  { delayTime2 *= clamp( inputs[DELAY2_INPUT].value / 10.0f, 0.0f, 1.0f );};

	if( inputs[LENGTH1_INPUT].active ) { length1    *= clamp( inputs[LENGTH1_INPUT].value / 10.0f, minLength, 1.0f );};
	if( inputs[LENGTH2_INPUT].active ) { length2    *= clamp( inputs[LENGTH2_INPUT].value / 10.0f, minLength, 1.0f );};




	if( inputs[GATE1_INPUT].active ) {
	       
		if(gateTrigger1.process(inputs[GATE1_INPUT].value)) {
			delay1.trigger(delayTime1);
			gate1 = true;
		};

	};

	if( inputs[GATE2_INPUT].active ) {
	       
		if(gateTrigger2.process(inputs[GATE2_INPUT].value)) {
			delay2.trigger(delayTime2);
			gate2 = true;
		};

	};


	if(  gate1 && !delay1.process(1.0/gSampleRate) ) {
			
		on1.trigger(length1);
		gate1 = false;

	};

	if(  gate2 && !delay2.process(1.0/gSampleRate) ) {
			
		on2.trigger(length2);
		gate2 = false;

	};

	outputs[OUT1_OUTPUT].value = on1.process(1.0/gSampleRate) ? 10.0 : 0.0;
	outputs[OUT2_OUTPUT].value = on2.process(1.0/gSampleRate) ? 10.0 : 0.0;

};



struct TrigDelayWidget : ModuleWidget {
	TrigDelayWidget(TrigDelay *module);
};

TrigDelayWidget::TrigDelayWidget(TrigDelay *module) : ModuleWidget(module) {

	box.size = Vec(15*6, 380);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(pluginInstance,"res/TrigDelay.svg")));
		addChild(panel);
	}

	addChild(createWidget<MLScrew>(Vec(15, 0)));
	addChild(createWidget<MLScrew>(Vec(15, 365)));

    addParam(createParam<SmallBlueMLKnob>(Vec(12,  69), module, TrigDelay::DELAY1_PARAM, 0.0, 2.0, 0.0));
	addInput(createPort<MLPort>(Vec(52, 70), PortWidget::INPUT, module, TrigDelay::DELAY1_INPUT));

    addParam(createParam<SmallBlueMLKnob>(Vec(12,  112), module, TrigDelay::LENGTH1_PARAM, minLength, 2.0, 0.1));
	addInput(createPort<MLPort>(Vec(52, 113), PortWidget::INPUT, module, TrigDelay::LENGTH1_INPUT));

	addInput(createPort<MLPort>(Vec(12, 164), PortWidget::INPUT, module, TrigDelay::GATE1_INPUT));
	addOutput(createPort<MLPort>(Vec(52, 164), PortWidget::OUTPUT, module, TrigDelay::OUT1_OUTPUT));

    addParam(createParam<SmallBlueMLKnob>(Vec(12,  153 + 69),  module, TrigDelay::DELAY2_PARAM, 0.0, 2.0, 0.0));
	addInput(createPort<MLPort>(Vec(52, 152 + 71), PortWidget::INPUT, module, TrigDelay::DELAY2_INPUT));

    addParam(createParam<SmallBlueMLKnob>(Vec(12,  153 + 112), module, TrigDelay::LENGTH2_PARAM, minLength, 2.0, 0.1));
	addInput(createPort<MLPort>(Vec(52, 152 + 114), PortWidget::INPUT, module, TrigDelay::LENGTH2_INPUT));

	addInput(createPort<MLPort>(Vec(12, 152 + 165), PortWidget::INPUT, module, TrigDelay::GATE2_INPUT));
	addOutput(createPort<MLPort>(Vec(52, 152 + 165), PortWidget::OUTPUT, module, TrigDelay::OUT2_OUTPUT));

}

Model *modelTrigDelay = createModel<TrigDelay, TrigDelayWidget>("TrigDelay");
