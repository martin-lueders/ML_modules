#include "ML_modules.hpp"


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


	Sum8mk2() { 
		config( NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS ) ;
		for(int i=0; i<8; i++) {
		    configParam(Sum8mk2::POLARITY_PARAM + i, 0.0, 1.0, 1.0);
		};
	};


	void process(const ProcessArgs &args) override;

};



void Sum8mk2::process(const ProcessArgs &args) {

	float out[PORT_MAX_CHANNELS];
	float in[PORT_MAX_CHANNELS];

	int channels[8];
	int max_channels = 0;

	for(int i=0; i<8; i++) {
		int c = channels[i] = inputs[IN_INPUT+i].getChannels();
		max_channels = c>max_channels?c:max_channels;
	}

	outputs[OUT_OUTPUT].setChannels(max_channels);

	for(int i=0; i<8; i++) {
		if(inputs[IN_INPUT+i].isConnected()) {
			float sign = (2*params[POLARITY_PARAM+i].getValue() - 1.0);
			inputs[IN_INPUT+i].readVoltages(in);
			for(int c=0; c<channels[i]; c++) out[c]+=in[c]  * sign;
		}
	}
	outputs[OUT_OUTPUT].writeVoltages(out);

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


	addOutput(createOutput<MLPort>(Vec(offset_x, 320), module, Sum8mk2::OUT_OUTPUT));


}

Model *modelSum8mk2 = createModel<Sum8mk2, Sum8mk2Widget>("Sum8mk2");
