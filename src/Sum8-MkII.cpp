#include "ML_modules.hpp"
#include "simd_mask.hpp"


using simd::float_4;


struct Sum8mk2 : Module {
	enum ParamIds {
		POLARITY_PARAM,
		NUM_PARAMS = POLARITY_PARAM + 8
	};
	enum InputIds {
		IN_INPUT,
		NUM_INPUTS = IN_INPUT + 8
	};
	enum OutputIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	ChannelMask channelMask;
	
	Sum8mk2() { 
		config( NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS );

		for(int i=0; i<8; i++) {
			configInput(Sum8mk2::IN_INPUT+i, "CV");
		    configSwitch(Sum8mk2::POLARITY_PARAM + i, 0.0, 1.0, 1.0, "Sign", {"-", "+"});
		};
		configOutput(OUT_OUTPUT, "CV Sum");
	};


	void process(const ProcessArgs &args) override;

};



void Sum8mk2::process(const ProcessArgs &args) {

	float_4 out[4];
	float_4 in[4];

	int channels[8];
	int max_channels = 1;

	for(int i=0; i<8; i++) {
		int c = channels[i] = inputs[IN_INPUT+i].getChannels();
		max_channels = c>max_channels?c:max_channels;
	}

	memset(out, 0, sizeof(out));

	outputs[OUT_OUTPUT].setChannels(max_channels);

	for(int i=0; i<8; i++) {
		if(inputs[IN_INPUT+i].isConnected()) {
			float_4 sign = float_4(2*params[POLARITY_PARAM+i].getValue() - 1.0f);
//			load_input(inputs[IN_INPUT+i], in, channels[i]);
			for(int c=0; c<channels[i]; c+=4) {
				in[c/4] = inputs[IN_INPUT+i].getPolyVoltageSimd<simd::float_4>(c);
			}
			channelMask.apply(in, channels[i]);
			for(int c=0; c<channels[i]; c+=4) out[c/4]+=in[c/4] * sign;
		}
	}
	for(int c=0; c<max_channels; c+=4) out[c/4].store(outputs[OUT_OUTPUT].getVoltages(c));

};



struct Sum8mk2Widget : ModuleWidget {
	Sum8mk2Widget(Sum8mk2 *module);
};

Sum8mk2Widget::Sum8mk2Widget(Sum8mk2 *module) {
		setModule(module);

	box.size = Vec(15*5, 380);

	{
		SvgPanel *panel = new SvgPanel();
		panel->box.size = box.size;
		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance,"res/Sum8mk2.svg")));

		addChild(panel);
	}

	addChild(createWidget<MLScrew>(Vec(15, 0)));
	addChild(createWidget<MLScrew>(Vec(15, 365)));




	const float offset_y = 70, delta_y = 26.5, offset_x=9.5;

	for( int i=0; i<8; i++) {
		addInput(createInput<MLPort>(Vec(offset_x, offset_y + i*delta_y  ), module, Sum8mk2::IN_INPUT+i));
        addParam(createParam<POLSWITCH>( Vec(offset_x + 37, offset_y + i*delta_y + 2 ), module, Sum8mk2::POLARITY_PARAM + i));
	}


	addOutput(createOutput<MLPortOut>(Vec(offset_x, 320), module, Sum8mk2::OUT_OUTPUT));


}

Model *modelSum8mk2 = createModel<Sum8mk2, Sum8mk2Widget>("Sum8mk2");
