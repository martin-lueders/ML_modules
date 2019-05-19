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

	Quant() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);};

	void process(const ProcessArgs &args) override;
};


static void stepChannel(Input &in, Param &p_amount, Output &out) {



	float v = in.normalize(0.0);

	float amount = p_amount.value;

	int octave = round(v);

	float rest = v - 1.0*octave;

	int semi   = round( rest*12.0 );

	float quantized = 1.0*octave + semi/12.0;

	out.value = quantized + amount*(v-quantized);

}

void Quant::process(const ProcessArgs &args) {
	if (outputs[OUT1_OUTPUT].isConnected()) stepChannel(inputs[IN1_INPUT], params[AMOUNT1_PARAM], outputs[OUT1_OUTPUT]);
	if (outputs[OUT2_OUTPUT].isConnected()) stepChannel(inputs[IN2_INPUT], params[AMOUNT2_PARAM], outputs[OUT2_OUTPUT]);
}


struct QuantizerWidget : ModuleWidget {
	QuantizerWidget(Quant *module);
};

QuantizerWidget::QuantizerWidget(Quant *module) {
		setModule(module);
	box.size = Vec(15*3, 380);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance,"res/Quantizer.svg")));
		addChild(panel);
	}

	addChild(createWidget<MLScrew>(Vec(15, 0)));
	addChild(createWidget<MLScrew>(Vec(15, 365)));

	addParam(createParam<SmallBlueMLKnob>(Vec(9,  60), module, Quant::AMOUNT1_PARAM, -1.0, 1.0, 0.0));
	addInput( createPort<MLPort>(Vec(9, 104), module, Quant::IN1_INPUT));
	addOutput(createOutput<MLPort>(Vec(9, 150), module, Quant::OUT1_OUTPUT));

	addParam(createParam<SmallBlueMLKnob>(Vec(9, 203), module, Quant::AMOUNT2_PARAM, -1.0, 1.0, 0.0));
	addInput( createPort<MLPort>(Vec(9, 246), module, Quant::IN2_INPUT));
	addOutput(createOutput<MLPort>(Vec(9, 292), module, Quant::OUT2_OUTPUT));
}

Model *modelQuantizer = createModel<Quant, QuantizerWidget>("Quantizer");