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


	TrigDelay() {
		config( NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS );
        configParam(TrigDelay::DELAY1_PARAM, 0.0, 2.0, 0.0);
        configParam(TrigDelay::LENGTH1_PARAM, minLength, 2.0, 0.1);
        configParam(TrigDelay::DELAY2_PARAM, 0.0, 2.0, 0.0);
        configParam(TrigDelay::LENGTH2_PARAM, minLength, 2.0, 0.1);


		// minLength = 0.001;
	};


	void process(const ProcessArgs &args) override;

	bool gate1[PORT_MAX_CHANNELS], gate2[PORT_MAX_CHANNELS];

	dsp::SchmittTrigger gateTrigger1[PORT_MAX_CHANNELS], gateTrigger2[PORT_MAX_CHANNELS];
	dsp::PulseGenerator delay1[PORT_MAX_CHANNELS],  delay2[PORT_MAX_CHANNELS];
	dsp::PulseGenerator on1[PORT_MAX_CHANNELS], on2[PORT_MAX_CHANNELS];


	void onReset() override {

		for(int c=0; c<PORT_MAX_CHANNELS; c++) {
			gate1[c]=false;
			gate2[c]=false;
		}
	};

};




void TrigDelay::process(const ProcessArgs &args) {

	float gSampleTime = args.sampleTime;

	// channel-independent parts:

	float delayTime1 = params[DELAY1_PARAM].getValue();
	float delayTime2 = params[DELAY2_PARAM].getValue();
	float length1    = params[LENGTH1_PARAM].getValue();
	float length2    = params[LENGTH2_PARAM].getValue();


	if( inputs[DELAY1_INPUT].isConnected() )  { delayTime1 *= clamp( inputs[DELAY1_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f );};
	if( inputs[DELAY2_INPUT].isConnected() )  { delayTime2 *= clamp( inputs[DELAY2_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f );};

	if( inputs[LENGTH1_INPUT].isConnected() ) { length1    *= clamp( inputs[LENGTH1_INPUT].getVoltage() / 10.0f, minLength, 1.0f );};
	if( inputs[LENGTH2_INPUT].isConnected() ) { length2    *= clamp( inputs[LENGTH2_INPUT].getVoltage() / 10.0f, minLength, 1.0f );};


	int channels_1 = inputs[GATE1_INPUT].getChannels();
	int channels_2 = inputs[GATE2_INPUT].getChannels();

	outputs[OUT1_OUTPUT].setChannels(channels_1);
	outputs[OUT2_OUTPUT].setChannels(channels_2);

	for(int c=0; c<channels_1; c++) {

		if(gateTrigger1[c].process( inputs[GATE1_INPUT].getNormalPolyVoltage(0.0f, c) ) ) {
			delay1[c].trigger(delayTime1);
			gate1[c] = true;
		};
	
		if(  gate1[c] && !delay1[c].process(gSampleTime) ) {
			on1[c].trigger(length1);
			gate1[c] = false;
		};

		outputs[OUT1_OUTPUT].setVoltage(on1[c].process(gSampleTime) ? 10.0 : 0.0, c);

	};

	for(int c=0; c<channels_2; c++) {
	       
		if(gateTrigger2[c].process(inputs[GATE2_INPUT].getNormalPolyVoltage(0.0f, c))) {
			delay2[c].trigger(delayTime2);
			gate2[c] = true;
		};
	
		if(  gate2[c] && !delay2[c].process(gSampleTime) ) {
			on2[c].trigger(length2);
			gate2[c] = false;
		};

	outputs[OUT2_OUTPUT].setVoltage(on2[c].process(gSampleTime) ? 10.0 : 0.0, c);
	};




};



struct TrigDelayWidget : ModuleWidget {
	TrigDelayWidget(TrigDelay *module);
};

TrigDelayWidget::TrigDelayWidget(TrigDelay *module) {
		
	setModule(module);

	box.size = Vec(15*6, 380);

	{
		SvgPanel *panel = new SvgPanel();
		panel->box.size = box.size;
		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance,"res/TrigDelay.svg")));
		addChild(panel);
	}

	addChild(createWidget<MLScrew>(Vec(15, 0)));
	addChild(createWidget<MLScrew>(Vec(15, 365)));

    addParam(createParam<SmallBlueMLKnob>(Vec(12,  69), module, TrigDelay::DELAY1_PARAM));
	addInput(createInput<MLPort>(Vec(52, 70), module, TrigDelay::DELAY1_INPUT));

    addParam(createParam<SmallBlueMLKnob>(Vec(12,  112), module, TrigDelay::LENGTH1_PARAM));
	addInput(createInput<MLPort>(Vec(52, 113), module, TrigDelay::LENGTH1_INPUT));

	addInput(createInput<MLPort>(Vec(12, 164), module, TrigDelay::GATE1_INPUT));
	addOutput(createOutput<MLPort>(Vec(52, 164), module, TrigDelay::OUT1_OUTPUT));

    addParam(createParam<SmallBlueMLKnob>(Vec(12,  153 + 69),  module, TrigDelay::DELAY2_PARAM));
	addInput(createInput<MLPort>(Vec(52, 152 + 71), module, TrigDelay::DELAY2_INPUT));

    addParam(createParam<SmallBlueMLKnob>(Vec(12,  153 + 112), module, TrigDelay::LENGTH2_PARAM));
	addInput(createInput<MLPort>(Vec(52, 152 + 114), module, TrigDelay::LENGTH2_INPUT));

	addInput(createInput<MLPort>(Vec(12, 152 + 165), module, TrigDelay::GATE2_INPUT));
	addOutput(createOutput<MLPort>(Vec(52, 152 + 165), module, TrigDelay::OUT2_OUTPUT));

}


Model *modelTrigDelay = createModel<TrigDelay, TrigDelayWidget>("TrigDelay");
