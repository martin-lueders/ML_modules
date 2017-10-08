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

	revmodel reverb;

	float roomsize, damp, freezelight;
	bool freeze=false;

	SchmittTrigger buttonTrigger;

#ifdef v032
	float old_samplerate;
#endif

#ifdef v032
	FreeVerb() ;
#endif
#ifdef v040
	FreeVerb() : Module( NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS ) {

		reverb.init(gSampleRate);

	};
#endif

	void step();

	void onSampleRateChange();

	void initialize(){
	};

};

#ifdef v032
FreeVerb::FreeVerb() {

	params.resize(NUM_PARAMS);
	inputs.resize(NUM_INPUTS);
	outputs.resize(NUM_OUTPUTS);

	reverb.init(gSampleRate);

};
#endif

void FreeVerb::onSampleRateChange() {

	reverb.init(gSampleRate);

};

void FreeVerb::step() {

	float out1, out2;

	out1 = out2 = 0.0;

	float old_roomsize = roomsize;
	float old_damp = damp;
	bool  old_freeze = freeze;


#ifdef v032
	float input = clampf(getf(inputs[IN_INPUT]),-10.0,10.0);

	if(inputs[ROOMSIZE_INPUT]) {
		roomsize = clampf(getf(inputs[ROOMSIZE_INPUT])/8.0, 0.0, 1.0);
	} else {
		roomsize = params[ROOMSIZE_PARAM];
	};

	if(inputs[DAMP_INPUT]) {
		damp = clampf(getf(inputs[DAMP_INPUT])/8.0, 0.0, 1.0);
	} else {
		damp = params[DAMP_PARAM];
	};

	if(inputs[FREEZE_INPUT]) {
		freeze = getf(inputs[FREEZE_INPUT]) > 1.0;
	} else {
		if(buttonTrigger.process(params[FREEZE_PARAM])) freeze = !freeze;
	};

	if(old_samplerate != gSampleRate) reverb.init(gSampleRate);
	old_samplerate = gSampleRate;
#endif

#ifdef v040
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
		if(buttonTrigger.process(params[FREEZE_INPUT].value)) freeze = !freeze;
	};

#endif

	if( old_damp != damp ) reverb.setdamp(damp);
	if( old_roomsize != roomsize) reverb.setroomsize(roomsize);

	freezelight=freeze?10.0:0.0;

	if(freeze != old_freeze) reverb.setmode(freeze?1.0:0.0);

	reverb.process(input, out1, out2);


#ifdef v032
	setf(outputs[OUT1_OUTPUT],out1);
	setf(outputs[OUT2_OUTPUT],out2);
#endif

#ifdef v040
	outputs[OUT1_OUTPUT].value = out1;
	outputs[OUT2_OUTPUT].value = out2;
#endif
};



FreeVerbWidget::FreeVerbWidget() {

	FreeVerb *module = new FreeVerb();
	setModule(module);
	box.size = Vec(15*6, 380);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
#ifdef v032
		panel->setBackground(SVG::load("plugins/ML_modules/res/FreeVerb.svg"));
#endif
#ifdef v040
		panel->setBackground(SVG::load(assetPlugin(plugin,"res/FreeVerb.svg")));
#endif
		addChild(panel);
	}

	addChild(createScrew<ScrewSilver>(Vec(15, 0)));
	addChild(createScrew<ScrewSilver>(Vec(15, 365)));

	addInput(createInput<PJ301MPort>(Vec(33, 50),    module, FreeVerb::IN_INPUT));


	addInput(createInput<PJ301MPort>(Vec(53, 120),    module, FreeVerb::ROOMSIZE_INPUT));
	addInput(createInput<PJ301MPort>(Vec(53, 183),    module, FreeVerb::DAMP_INPUT));
	addInput(createInput<PJ301MPort>(Vec(53, 246),    module, FreeVerb::FREEZE_INPUT));



        addParam(createParam<Davies1900hSmallBlackKnob>(Vec(10, 122), module, FreeVerb::ROOMSIZE_PARAM, 0.0, 1.0, 0.5));
        addParam(createParam<Davies1900hSmallBlackKnob>(Vec(10, 186), module, FreeVerb::DAMP_PARAM, 0.0, 1.0, 0.5));
        addParam(createParam<LEDButton>(Vec(14, 250), module, FreeVerb::FREEZE_PARAM, 0.0, 10.0, 0.0));
        addChild(createValueLight<SmallLight<GreenValueLight>>(Vec(19,255), &module->freezelight));

	addOutput(createOutput<PJ301MPort>(Vec(11, 313), module, FreeVerb::OUT1_OUTPUT));
	addOutput(createOutput<PJ301MPort>(Vec(55, 313), module, FreeVerb::OUT2_OUTPUT));
}
