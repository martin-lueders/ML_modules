#include "ML_modules.hpp"
#include "../freeverb/revmodel.hpp"


struct FreeVerb : Module {
	enum ParamIds {
		ROOMSIZE_PARAM,
		DAMP_PARAM,
		FREEZE_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		IN_INPUT,
		ROOMSIZE_INPUT,
		DAMP_INPUT,
		FREEZE_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightIds {
		FREEZE_LIGHT,
		NUM_LIGHTS
	};
	revmodel reverb;

	float roomsize, damp; 


	bool freeze=false;

	dsp::SchmittTrigger buttonTrigger;

	FreeVerb() {
		config( NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS );

		configInput(IN_INPUT, "Audio" );
		configInput(ROOMSIZE_INPUT, "Roomsize" );
		configInput(DAMP_INPUT, "Damping" );
		configInput(FREEZE_INPUT, "Freeze" );

		configOutput(OUT1_OUTPUT, "Left");
		configOutput(OUT2_OUTPUT, "Right");

        configParam(FreeVerb::ROOMSIZE_PARAM, 0.0, 1.0, 0.5, "Roomsize" );
        configParam(FreeVerb::DAMP_PARAM, 0.0, 1.0, 0.5, "Damping" );
        configButton(FreeVerb::FREEZE_PARAM, "Freeze" );

		float gSampleRate = APP->engine->getSampleRate();

		reverb.init(gSampleRate);

	};

	void process(const ProcessArgs &args) override;

	void onSampleRateChange() override;


};


void FreeVerb::onSampleRateChange() {

	float gSampleRate = APP->engine->getSampleRate();

	reverb.init(gSampleRate);

	reverb.setdamp(damp);
	reverb.setroomsize(roomsize);

};

void FreeVerb::process(const ProcessArgs &args) {

	float out1, out2;

	out1 = out2 = 0.0;

	float old_roomsize = roomsize;
	float old_damp = damp;
	bool  old_freeze = freeze;


	float input = clamp(inputs[IN_INPUT].getVoltage(),-10.0f,10.0f);

	if(inputs[ROOMSIZE_INPUT].isConnected()) {
		roomsize = clamp(inputs[ROOMSIZE_INPUT].getVoltage()/8.0f, 0.0f, 1.0f);
	} else {
		roomsize = params[ROOMSIZE_PARAM].getValue();
	};

	if(inputs[DAMP_INPUT].isConnected()) {
		damp     = clamp(inputs[DAMP_INPUT].getVoltage()/8.0f, 0.0f, 1.0f);
	} else {
		damp     = params[DAMP_PARAM].getValue();
	};

	if(inputs[FREEZE_INPUT].isConnected()) {
		freeze = inputs[FREEZE_INPUT].getVoltage() > 1.0;
	} else {
		if(buttonTrigger.process(params[FREEZE_PARAM].getValue())) freeze = !freeze;
	};


	if( old_damp != damp ) reverb.setdamp(damp);
	if( old_roomsize != roomsize) reverb.setroomsize(roomsize);

	lights[FREEZE_LIGHT].value = freeze?10.0:0.0;


	if(freeze != old_freeze) reverb.setmode(freeze?1.0:0.0);

	reverb.process(input, out1, out2);


	outputs[OUT1_OUTPUT].setVoltage(out1);
	outputs[OUT2_OUTPUT].setVoltage(out2);
};



struct FreeVerbWidget : ModuleWidget {
	FreeVerbWidget(FreeVerb *module);
};

FreeVerbWidget::FreeVerbWidget(FreeVerb *module) {
		setModule(module);

	box.size = Vec(15*6, 380);

	{
		SvgPanel *panel = new SvgPanel();
		panel->box.size = box.size;
		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance,"res/FreeVerb.svg")));
		addChild(panel);
	}

	addChild(createWidget<MLScrew>(Vec(15, 0)));
	addChild(createWidget<MLScrew>(Vec(15, 365)));

	addInput(createInput<MLPort>(Vec(33, 50), module, FreeVerb::IN_INPUT));

	addInput(createInput<MLPort>(Vec(53, 120), module, FreeVerb::ROOMSIZE_INPUT));
	addInput(createInput<MLPort>(Vec(53, 183), module, FreeVerb::DAMP_INPUT));
	addInput(createInput<MLPort>(Vec(53, 246), module, FreeVerb::FREEZE_INPUT));

        addParam(createParam<SmallBlueMLKnob>(Vec(10, 122), module, FreeVerb::ROOMSIZE_PARAM));
        addParam(createParam<SmallBlueMLKnob>(Vec(10, 186), module, FreeVerb::DAMP_PARAM));
        addParam(createParam<ML_MediumLEDButton>(Vec(14, 250), module, FreeVerb::FREEZE_PARAM));
        addChild(createLight<MLMediumLight<GreenLight>>(Vec(18,254), module, FreeVerb::FREEZE_LIGHT));



	addOutput(createOutput<MLPort>(Vec(11, 313), module, FreeVerb::OUT1_OUTPUT));
	addOutput(createOutput<MLPort>(Vec(55, 313), module, FreeVerb::OUT2_OUTPUT));
}

Model *modelFreeVerb = createModel<FreeVerb, FreeVerbWidget>("FreeVerb");
