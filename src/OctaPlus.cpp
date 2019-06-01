#include "ML_modules.hpp"


struct OctaPlus : Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		IN_A_INPUT,
		IN_B_INPUT = IN_A_INPUT + 8,
		NUM_INPUTS = IN_B_INPUT + 8
	};
	enum OutputIds {
		OUT_OUTPUT,
		NUM_OUTPUTS = OUT_OUTPUT + 8
	};
	enum LightIds {
		NUM_LIGHTS
	};


	OctaPlus() {
		config( NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS );
	};

	void process(const ProcessArgs &args) override;

};



void OctaPlus::process(const ProcessArgs &args) {

	float in_A[PORT_MAX_CHANNELS], in_B[PORT_MAX_CHANNELS], out[PORT_MAX_CHANNELS];

	int channels_A = 0, channels_B = 0, channels_OUT = 0;

	memset(in_A, 0, PORT_MAX_CHANNELS*sizeof(float));
	memset(in_B, 0, PORT_MAX_CHANNELS*sizeof(float));
	memset(out,  0, PORT_MAX_CHANNELS*sizeof(float));

	for(int i=0; i<8; i++) {

		if( inputs[IN_A_INPUT+i].isConnected() ) {
			channels_A = inputs[IN_A_INPUT+i].getChannels();
			inputs[IN_A_INPUT+i].readVoltages(in_A);
		}
		if( inputs[IN_B_INPUT+i].isConnected() ) {
			channels_B = inputs[IN_B_INPUT+i].getChannels();
			inputs[IN_B_INPUT+i].readVoltages(in_B);
		}
		channels_OUT = MAX(channels_A, channels_B);

		for(int c=0; c<channels_OUT; c++) out[c] = in_A[c] + in_B[c];
		outputs[OUT_OUTPUT+i].setChannels(channels_OUT);
		outputs[OUT_OUTPUT+i].writeVoltages(out);
	
	};

};



struct OctaPlusWidget : ModuleWidget {
	OctaPlusWidget(OctaPlus *module);
};

OctaPlusWidget::OctaPlusWidget(OctaPlus *module) {
		setModule(module);

	box.size = Vec(15*8, 380);

	{
		SvgPanel *panel = new SvgPanel();
		panel->box.size = box.size;
		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance,"res/OctaPlus.svg")));

		addChild(panel);
	}

	addChild(createWidget<MLScrew>(Vec(15, 0)));
	addChild(createWidget<MLScrew>(Vec(box.size.x-30, 0)));
	addChild(createWidget<MLScrew>(Vec(15, 365)));
	addChild(createWidget<MLScrew>(Vec(box.size.x-30, 365)));




	const float offset_y = 60, delta_y = 32, row1=15, row2 = 48, row3 = 80;

	for( int i=0; i<8; i++) {
		addInput(createInput<MLPort>(Vec(row1, offset_y + i*delta_y  ), module, OctaPlus::IN_A_INPUT+i));
		addInput(createInput<MLPort>(Vec(row2, offset_y + i*delta_y  ), module, OctaPlus::IN_B_INPUT+i));
		addOutput(createOutput<MLPort>(Vec(row3, offset_y + i*delta_y ), module, OctaPlus::OUT_OUTPUT+i));
	};


}

Model *modelOctaPlus = createModel<OctaPlus, OctaPlusWidget>("OctaPlus");
