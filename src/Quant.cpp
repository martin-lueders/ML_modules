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
	enum LighIds {
		NUM_LIGHTS
	};

	Quant() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {};

	void step() override;
};


static void stepChannel(Input &in, Param &p_amount, Output &out) {



	float v = in.value;

	float amount = p_amount.value;

	int octave = round(v);

	float rest = v - 1.0*octave;

	int semi   = round( rest*12.0 );

	float quantized = 1.0*octave + semi/12.0;
        float result = quantized + amount*(v-quantized);


	out.value = result;

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
		panel->setBackground(SVG::load(assetPlugin(plugin,"res/Quantizer.svg")));
		addChild(panel);
	}

	addChild(createScrew<ScrewSilver>(Vec(15, 0)));
	addChild(createScrew<ScrewSilver>(Vec(15, 365)));

	addParam(createParam<SmallMLKnob>(Vec(9,  60), module, Quant::AMOUNT1_PARAM, -1.0, 1.0, 0.0));
	addInput(createInput<PJ301MPort>(Vec(10, 105), module, Quant::IN1_INPUT));
	addOutput(createOutput<PJ301MPort>(Vec(10, 150), module, Quant::OUT1_OUTPUT));

	addParam(createParam<SmallMLKnob>(Vec(9, 203), module, Quant::AMOUNT2_PARAM, -1.0, 1.0, 0.0));
	addInput(createInput<PJ301MPort>(Vec(10, 245), module, Quant::IN2_INPUT));
	addOutput(createOutput<PJ301MPort>(Vec(10, 290), module, Quant::OUT2_OUTPUT));
}
