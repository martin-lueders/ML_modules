#include "ML_modules.hpp"
#include "simd_mask.hpp"


using simd::float_4;


struct Sum8 : Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		IN1_INPUT,
		IN2_INPUT,
		IN3_INPUT,
		IN4_INPUT,
		IN5_INPUT,
		IN6_INPUT,
		IN7_INPUT,
		IN8_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	ChannelMask channelMask;

	Sum8() {
		config( NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS );

		for(int i=0; i<8; i++) {
			configInput(IN1_INPUT+i, "CV");
		};
		configOutput(OUT_OUTPUT, "CV Sum");

	};


	void process(const ProcessArgs &args) override;

};



void Sum8::process(const ProcessArgs &args) {

	float_4 out[4];
	float_4 in[4];

	int channels[8];
	int max_channels = 1;

	for(int i=0; i<8; i++) {
		int c = channels[i] = inputs[IN1_INPUT+i].getChannels();
		max_channels = c>max_channels?c:max_channels;
	}

	outputs[OUT_OUTPUT].setChannels(max_channels);

	memset(out, 0, sizeof(out));

	for(int i=0; i<8; i++) {
		if(inputs[IN1_INPUT+i].isConnected()) {
			load_input(inputs[IN1_INPUT+i], in, channels[i]);
			channelMask.apply(in, channels[i]);
			for(int c=0; c<channels[i]; c+=4) out[c/4]+=in[c/4];
		}
	}

	for(int c=0; c<max_channels; c+=4) out[c/4].store(outputs[OUT_OUTPUT].getVoltages(c));

};



struct Sum8Widget : ModuleWidget {
	Sum8Widget(Sum8 *module);
};

Sum8Widget::Sum8Widget(Sum8 *module) {
		setModule(module);

	box.size = Vec(15*3, 380);

	{
		SvgPanel *panel = new SvgPanel();
		panel->box.size = box.size;
		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance,"res/Sum8.svg")));

		addChild(panel);
	}

	addChild(createWidget<MLScrew>(Vec(15, 0)));
	addChild(createWidget<MLScrew>(Vec(15, 365)));




	const float offset_y = 70, delta_y = 26.5, offset_x=9.5;

	for( int i=0; i<8; i++) addInput(createInput<MLPort>(Vec(offset_x, offset_y + i*delta_y  ), module, Sum8::IN1_INPUT+i));


	addOutput(createOutput<MLPortOut>(Vec(offset_x, 320), module, Sum8::OUT_OUTPUT));


}

Model *modelSum8 = createModel<Sum8, Sum8Widget>("Sum8");
