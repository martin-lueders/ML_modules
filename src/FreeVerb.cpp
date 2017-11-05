#include "ML_modules.hpp"
#include "../freeverb/revmodel.hpp"

#include "dsp/digital.hpp"

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

#ifdef v_dev
	enum LighIds {
		FREEZE_LIGHT,
		NUM_LIGHTS
	};
#endif
	revmodel reverb;

	float roomsize, damp; 

#ifdef v040
	float freezelight;
#endif

	bool freeze=false;

	SchmittTrigger buttonTrigger;


	FreeVerb() : Module( NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS ) {

#ifdef v_dev
	float gSampleRate = engineGetSampleRate();
#endif	

		reverb.init(gSampleRate);

	};

	void step() override;

	void onSampleRateChange() override;


};


void FreeVerb::onSampleRateChange() {

#ifdef v_dev
	float gSampleRate = engineGetSampleRate();
#endif	

	reverb.init(gSampleRate);

};

void FreeVerb::step() {

	float out1, out2;

	out1 = out2 = 0.0;

	float old_roomsize = roomsize;
	float old_damp = damp;
	bool  old_freeze = freeze;


	float input = clampf(inputs[IN_INPUT].value,-10.0,10.0);

	if(inputs[ROOMSIZE_INPUT].active) {
		roomsize = clampf(inputs[ROOMSIZE_INPUT].value/8.0, 0.0, 1.0);
	} else {
		roomsize = params[ROOMSIZE_PARAM].value;
	};

	if(inputs[DAMP_INPUT].active) {
		damp     = clampf(inputs[DAMP_INPUT].value/8.0, 0.0, 1.0);
	} else {
		damp     = params[DAMP_PARAM].value;
	};

	if(inputs[FREEZE_INPUT].active) {
		freeze = inputs[FREEZE_INPUT].value > 1.0;
	} else {
		if(buttonTrigger.process(params[FREEZE_PARAM].value)) freeze = !freeze;
	};


	if( old_damp != damp ) reverb.setdamp(damp);
	if( old_roomsize != roomsize) reverb.setroomsize(roomsize);

#ifdef v040
	freezelight=freeze?10.0:0.0;
#endif

#ifdef v_dev
	lights[FREEZE_LIGHT].value = freeze?10.0:0.0;
#endif


	if(freeze != old_freeze) reverb.setmode(freeze?1.0:0.0);

	reverb.process(input, out1, out2);


	outputs[OUT1_OUTPUT].value = out1;
	outputs[OUT2_OUTPUT].value = out2;
};



FreeVerbWidget::FreeVerbWidget() {

	FreeVerb *module = new FreeVerb();
	setModule(module);
	box.size = Vec(15*6, 380);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin,"res/FreeVerb.svg")));
		addChild(panel);
	}

	addChild(createScrew<ScrewSilver>(Vec(15, 0)));
	addChild(createScrew<ScrewSilver>(Vec(15, 365)));

	addInput(createInput<PJ301MPort>(Vec(33, 50),    module, FreeVerb::IN_INPUT));


	addInput(createInput<PJ301MPort>(Vec(53, 120),    module, FreeVerb::ROOMSIZE_INPUT));
	addInput(createInput<PJ301MPort>(Vec(53, 183),    module, FreeVerb::DAMP_INPUT));
	addInput(createInput<PJ301MPort>(Vec(53, 246),    module, FreeVerb::FREEZE_INPUT));



        addParam(createParam<SmallMLKnob>(Vec(10, 122), module, FreeVerb::ROOMSIZE_PARAM, 0.0, 1.0, 0.5));
        addParam(createParam<SmallMLKnob>(Vec(10, 186), module, FreeVerb::DAMP_PARAM, 0.0, 1.0, 0.5));
        addParam(createParam<LEDButton>(Vec(14, 250), module, FreeVerb::FREEZE_PARAM, 0.0, 10.0, 0.0));
#ifdef v040
        addChild(createValueLight<SmallLight<GreenValueLight>>(Vec(19,255), &module->freezelight));
#endif
#ifdef v_dev
        addChild(createLight<SmallLight<GreenLight>>(Vec(19,255), module, FreeVerb::FREEZE_LIGHT));
#endif

	addOutput(createOutput<PJ301MPort>(Vec(11, 313), module, FreeVerb::OUT1_OUTPUT));
	addOutput(createOutput<PJ301MPort>(Vec(55, 313), module, FreeVerb::OUT2_OUTPUT));
}
