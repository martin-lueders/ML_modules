#include "ML_modules.hpp"
#include "simd_mask.hpp"

using simd::float_4;


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
        configParam(Quant::AMOUNT1_PARAM, -1.0, 1.0, 0.0, "Amount", "%", 0.0f, 100.f);
        configParam(Quant::AMOUNT2_PARAM, -1.0, 1.0, 0.0, "Amount", "%", 0.0f, 100.f);

		configInput(IN1_INPUT, "Pitch");
		configOutput(OUT1_OUTPUT, "Quantized pitch");

		configInput(IN2_INPUT, "Pitch");
		configOutput(OUT2_OUTPUT, "Quantized pitch");
	};

	void process(const ProcessArgs &args) override;
};


inline float_4 semi_quant_4(const float_4 &v, float amount) {

	using namespace simd;

	float_4 octave = round(v);
	float_4 rest = v - 1.0*octave;
	float_4 semi   = round( rest*12.0 );
	float_4 quantized = 1.0*octave + semi/12.0;
	float_4 result = quantized + amount*(v-quantized);

	return result;
}

void Quant::process(const ProcessArgs &args) {
	
	if (outputs[OUT1_OUTPUT].isConnected()) {
		float_4 in[4];
		float amount = params[AMOUNT1_PARAM].getValue();

		int channels = inputs[IN1_INPUT].getChannels();
		if(channels>0) {
			load_input(inputs[IN1_INPUT], in, channels);
			outputs[OUT1_OUTPUT].setChannels(channels);

			for(int c=0; c<channels; c+=4) {
				float_4 out = semi_quant_4(in[c/4], amount);
				out.store(outputs[OUT1_OUTPUT].getVoltages(c));
			}
		} else {
			outputs[OUT1_OUTPUT].setChannels(1);
			outputs[OUT1_OUTPUT].setVoltage(0.0f);
		};
	}
	
	if (outputs[OUT2_OUTPUT].isConnected()) {
		float_4 in[4];
		float amount = params[AMOUNT2_PARAM].getValue();

		int channels = inputs[IN2_INPUT].getChannels();
		if(channels>0) {
			load_input(inputs[IN2_INPUT], in, channels);
			outputs[OUT2_OUTPUT].setChannels(channels);

			for(int c=0; c<channels; c+=4) {
				float_4 out = semi_quant_4(in[c/4], amount);
				out.store(outputs[OUT2_OUTPUT].getVoltages(c));
			}
		} else {
			outputs[OUT2_OUTPUT].setChannels(1);
			outputs[OUT2_OUTPUT].setVoltage(0.0f);
		};
	}
	
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
	addOutput(createOutput<MLPortOut>(Vec(9, 150), module, Quant::OUT1_OUTPUT));

	addParam(createParam<SmallBlueMLKnob>(Vec(9, 203), module, Quant::AMOUNT2_PARAM));
	addInput(createInput<MLPort>(Vec(9, 246), module, Quant::IN2_INPUT));
	addOutput(createOutput<MLPortOut>(Vec(9, 292), module, Quant::OUT2_OUTPUT));
}



Model *modelQuantizer = createModel<Quant, QuantizerWidget>("Quantizer");
