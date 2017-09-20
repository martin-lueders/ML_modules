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

	Quant();
	void step();
};


Quant::Quant() {
	params.resize(NUM_PARAMS);
	inputs.resize(NUM_INPUTS);
	outputs.resize(NUM_OUTPUTS);
}

static void stepChannel(const float *in, float amount, float *out) {

	float v = getf(in);
// 	float v2 = v + 1.0/24.0; 

	// extract octave information (integer part of voltage):
	
//	int octave = floor(v2);
	int octave = round(v);

	float rest = v - 1.0*octave;

	int semi   = round( rest*12.0 );

	float quantized = 1.0*octave + semi/12.0;
        float result = quantized + amount*(v-quantized);

	setf(out, result);
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
		Panel *panel = new LightPanel();
		panel->box.size = box.size;
		panel->backgroundImage = Image::load("plugins/ML_modules/res/Quantizer.png");
		addChild(panel);
	}

	addChild(createScrew<ScrewSilver>(Vec(15, 0)));
	addChild(createScrew<ScrewSilver>(Vec(15, 365)));

	addParam(createParam<Davies1900hSmallBlackKnob>(Vec(9,  50), module, Quant::AMOUNT1_PARAM, -1.0, 1.0, 0.0));
	addInput(createInput<PJ301MPort>(Vec(10, 90), module, Quant::IN1_INPUT));
	addOutput(createOutput<PJ301MPort>(Vec(10, 130), module, Quant::OUT1_OUTPUT));

	addParam(createParam<Davies1900hSmallBlackKnob>(Vec(9, 200), module, Quant::AMOUNT2_PARAM, -1.0, 1.0, 0.0));
	addInput(createInput<PJ301MPort>(Vec(10, 240), module, Quant::IN2_INPUT));
	addOutput(createOutput<PJ301MPort>(Vec(10, 280), module, Quant::OUT2_OUTPUT));
}
