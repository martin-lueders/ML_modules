#include "ML_modules.hpp"


struct Quant : Module {
	enum ParamIds {
		AMOUNT1_PARAM,
		AMOUNT2_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		IN1_INPUT,
		IN2_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		NUM_OUTPUTS
	};

#ifdef v032
	Quant();
#endif
#ifdef v040
	Quant() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS) {};
#endif

	void step();
};

#ifdef v032
Quant::Quant() {
	params.resize(NUM_PARAMS);
	inputs.resize(NUM_INPUTS);
	outputs.resize(NUM_OUTPUTS);
};
#endif

#ifdef v032
static void stepChannel(const float *in, const float amount, float *out) {
#endif

#ifdef v040
static void stepChannel(Input &in, Param &p_amount, Output &out) {
#endif


#ifdef v032
	float v = getf(in);
#endif

#ifdef v040
	float v = in.value;

	float amount = p_amount.value;
#endif

// 	float v2 = v + 1.0/24.0; 

	// extract octave information (integer part of voltage):
	
//	int octave = floor(v2);
	int octave = round(v);

	float rest = v - 1.0*octave;

	int semi   = round( rest*12.0 );

	float quantized = 1.0*octave + semi/12.0;
        float result = quantized + amount*(v-quantized);

#ifdef v032
	setf(out, result);
#endif

#ifdef v040
	out.value = result;
#endif

}

void Quant::step() {
	stepChannel(inputs[IN1_INPUT], params[AMOUNT1_PARAM], outputs[OUT1_OUTPUT]);
	stepChannel(inputs[IN2_INPUT], params[AMOUNT2_PARAM], outputs[OUT2_OUTPUT]);
}


QuantizerWidget::QuantizerWidget() {
	Quant *module = new Quant();
	setModule(module);
	box.size = Vec(15*3, 380);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
#ifdef v032
		panel->setBackground(SVG::load("plugins/ML_modules/res/Quantizer.svg"));
#endif

#ifdef v040
		panel->setBackground(SVG::load(assetPlugin(plugin,"res/Quantizer.svg")));
#endif
		addChild(panel);
	}

	addChild(createScrew<ScrewSilver>(Vec(15, 0)));
	addChild(createScrew<ScrewSilver>(Vec(15, 365)));

	addParam(createParam<Davies1900hSmallBlackKnob>(Vec(9,  60), module, Quant::AMOUNT1_PARAM, -1.0, 1.0, 0.0));
	addInput(createInput<PJ301MPort>(Vec(10, 105), module, Quant::IN1_INPUT));
	addOutput(createOutput<PJ301MPort>(Vec(10, 150), module, Quant::OUT1_OUTPUT));

	addParam(createParam<Davies1900hSmallBlackKnob>(Vec(9, 203), module, Quant::AMOUNT2_PARAM, -1.0, 1.0, 0.0));
	addInput(createInput<PJ301MPort>(Vec(10, 245), module, Quant::IN2_INPUT));
	addOutput(createOutput<PJ301MPort>(Vec(10, 290), module, Quant::OUT2_OUTPUT));
}
