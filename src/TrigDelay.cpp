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

		gSampleRate = args.sampleRate;
		// minLength = 0.001;

	};


	void process(const ProcessArgs &args) override;

	bool gate1=false, gate2=false;

	dsp::SchmittTrigger gateTrigger1, gateTrigger2;
	dsp::PulseGenerator delay1,  delay2;
	dsp::PulseGenerator on1, on2;



	float gSampleRate;

	void onSampleRateChange() { gSampleRate = args.sampleRate; }

	void reset() override {

		gate1=false;
		gate2=false;
	};

private:



};




void TrigDelay::process(const ProcessArgs &args) {


	float delayTime1 = params[DELAY1_PARAM].getValue();
	float delayTime2 = params[DELAY2_PARAM].getValue();
	float length1    = params[LENGTH1_PARAM].getValue();
	float length2    = params[LENGTH2_PARAM].getValue();


	if( inputs[DELAY1_INPUT].isConnected() )  { delayTime1 *= clamp( inputs[DELAY1_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f );};
	if( inputs[DELAY2_INPUT].isConnected() )  { delayTime2 *= clamp( inputs[DELAY2_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f );};

	if( inputs[LENGTH1_INPUT].isConnected() ) { length1    *= clamp( inputs[LENGTH1_INPUT].getVoltage() / 10.0f, minLength, 1.0f );};
	if( inputs[LENGTH2_INPUT].isConnected() ) { length2    *= clamp( inputs[LENGTH2_INPUT].getVoltage() / 10.0f, minLength, 1.0f );};




	if( inputs[GATE1_INPUT].isConnected() ) {
	       
		if(gateTrigger1.process(inputs[GATE1_INPUT].getVoltage())) {
			delay1.trigger(delayTime1);
			gate1 = true;
		};

	};

	if( inputs[GATE2_INPUT].isConnected() ) {
	       
		if(gateTrigger2.process(inputs[GATE2_INPUT].getVoltage())) {
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

	outputs[OUT1_OUTPUT].setVoltage(on1.process(1.0/gSampleRate) ? 10.0 : 0.0);
	outputs[OUT2_OUTPUT].setVoltage(on2.process(1.0/gSampleRate) ? 10.0 : 0.0);

};



struct TrigDelayWidget : ModuleWidget {
	TrigDelayWidget(TrigDelay *module);
};

TrigDelayWidget::TrigDelayWidget(TrigDelay *module) {
		setModule(module);

	box.size = Vec(15*6, 380);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance,"res/TrigDelay.svg")));
		addChild(panel);
	}

	addChild(createWidget<MLScrew>(Vec(15, 0)));
	addChild(createWidget<MLScrew>(Vec(15, 365)));

    addParam(createParam<SmallBlueMLKnob>(Vec(12,  69), module, TrigDelay::DELAY1_PARAM, 0.0, 2.0, 0.0));
	addInput(createInput<MLPort>(Vec(52, 70), module, TrigDelay::DELAY1_INPUT));

    addParam(createParam<SmallBlueMLKnob>(Vec(12,  112), module, TrigDelay::LENGTH1_PARAM, minLength, 2.0, 0.1));
	addInput(createInput<MLPort>(Vec(52, 113), module, TrigDelay::LENGTH1_INPUT));

	addInput(createInput<MLPort>(Vec(12, 164), module, TrigDelay::GATE1_INPUT));
	addOutput(createOutput<MLPort>(Vec(52, 164), module, TrigDelay::OUT1_OUTPUT));

    addParam(createParam<SmallBlueMLKnob>(Vec(12,  153 + 69),  module, TrigDelay::DELAY2_PARAM, 0.0, 2.0, 0.0));
	addInput(createInput<MLPort>(Vec(52, 152 + 71), module, TrigDelay::DELAY2_INPUT));

    addParam(createParam<SmallBlueMLKnob>(Vec(12,  153 + 112), module, TrigDelay::LENGTH2_PARAM, minLength, 2.0, 0.1));
	addInput(createInput<MLPort>(Vec(52, 152 + 114), module, TrigDelay::LENGTH2_INPUT));

	addInput(createInput<MLPort>(Vec(12, 152 + 165), module, TrigDelay::GATE2_INPUT));
	addOutput(createOutput<MLPort>(Vec(52, 152 + 165), module, TrigDelay::OUT2_OUTPUT));

}

Model *modelTrigDelay = createModel<TrigDelay, TrigDelayWidget>("TrigDelay");
