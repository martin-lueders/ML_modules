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
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(Quant::AMOUNT1_PARAM, -1.0, 1.0, 0.0);
        configParam(Quant::AMOUNT2_PARAM, -1.0, 1.0, 0.0);
	};

	void process(const ProcessArgs &args) override;
};


static void stepPart(Input &in, Param &p_amount, Output &out) {

	using namespace simd;

	int channels = in.getChannels();
	int channels_padded = channels + (4-channels%4);

	float in_values[channels_padded];
	float out_values[channels_padded];

	out.setChannels(channels);

	float amount = p_amount.value;
	
	if (in.isConnected()) 	in.readVoltages(in_values);
	else                    memset(in_values, 0, channels_padded*sizeof(float));

	for(int batch=0; batch<channels_padded/4; batch++) {

		float_4 v=float_4::load(&in_values[4*batch]);
		float_4 octave = round(v);
		float_4 rest = v - 1.0*octave;
		float_4 semi   = round( rest*12.0 );
		float_4 quantized = 1.0*octave + semi/12.0;
		float_4 result = quantized + amount*(v-quantized);
		result.store(&out_values[4*batch]);
	}

	out.writeVoltages(out_values);
}

void Quant::process(const ProcessArgs &args) {
	if (outputs[OUT1_OUTPUT].isConnected()) stepPart(inputs[IN1_INPUT], params[AMOUNT1_PARAM], outputs[OUT1_OUTPUT]);
	if (outputs[OUT2_OUTPUT].isConnected()) stepPart(inputs[IN2_INPUT], params[AMOUNT2_PARAM], outputs[OUT2_OUTPUT]);
}


struct QuantizerWidget : ModuleWidget {
	QuantizerWidget(Quant *module);
};

QuantizerWidget::QuantizerWidget(Quant *module) {

	setModule(module);
	box.size = Vec(15*3, 380);

	{
		SvgPanel *panel = new SvgPanel();
		panel->box.size = box.size;
		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance,"res/Quantizer.svg")));
		addChild(panel);
	}

	addChild(createWidget<MLScrew>(Vec(15, 0)));
	addChild(createWidget<MLScrew>(Vec(15, 365)));

	addParam(createParam<SmallBlueMLKnob>(Vec(9,  60), module, Quant::AMOUNT1_PARAM));
	addInput(createInput<MLPort>(Vec(9, 104), module, Quant::IN1_INPUT));
	addOutput(createOutput<MLPort>(Vec(9, 150), module, Quant::OUT1_OUTPUT));

	addParam(createParam<SmallBlueMLKnob>(Vec(9, 203), module, Quant::AMOUNT2_PARAM));
	addInput(createInput<MLPort>(Vec(9, 246), module, Quant::IN2_INPUT));
	addOutput(createOutput<MLPort>(Vec(9, 292), module, Quant::OUT2_OUTPUT));
}



Model *modelQuantizer = createModel<Quant, QuantizerWidget>("Quantizer");
