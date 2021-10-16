#include "ML_modules.hpp"
#include "simd_mask.hpp"

using simd::float_4;


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

	ChannelMask channelMask;

	OctaPlus() {
		config( NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS );

		for(int i=0; i<8; i++) {
			configInput(IN_A_INPUT+i, "A #"+std::to_string(i+1));
			configInput(IN_B_INPUT+i, "B #"+std::to_string(i+1));
			configOutput(OUT_OUTPUT+i, "#"+std::to_string(i+1));
		}
	};

	void process(const ProcessArgs &args) override;

};



void OctaPlus::process(const ProcessArgs &args) {

	float_4 in_A[4], in_B[4], out[4];

	int channels_A = 0, channels_B = 0, channels_OUT = 0;

	memset(in_A, 0, sizeof(in_A));
	memset(in_B, 0, sizeof(in_B));
	memset(out,  0, sizeof(out));

	for(int i=0; i<8; i++) {

		int tmp_A = inputs[IN_A_INPUT+i].getChannels();
		int tmp_B = inputs[IN_B_INPUT+i].getChannels();

		channels_A = tmp_A>0 ? tmp_A : channels_A; 
		channels_B = tmp_B>0 ? tmp_B : channels_B; 

		load_input(inputs[IN_A_INPUT+i], in_A, tmp_A);
		if(tmp_A>1) channelMask.apply_all(in_A, tmp_A);

		load_input(inputs[IN_B_INPUT+i], in_B, tmp_B);
		if(tmp_B>1) channelMask.apply_all(in_B, tmp_B);

		channels_OUT = MAX(channels_A, channels_B);

		for(int c=0; c<channels_OUT; c+=4) out[c/4] = in_A[c/4] + in_B[c/4];

		outputs[OUT_OUTPUT+i].setChannels(channels_OUT);
		for(int c=0; c<channels_OUT; c+=4) out[c/4].store(outputs[OUT_OUTPUT+i].getVoltages(c));	
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
		addOutput(createOutput<MLPortOut>(Vec(row3, offset_y + i*delta_y ), module, OctaPlus::OUT_OUTPUT+i));
	};


}

Model *modelOctaPlus = createModel<OctaPlus, OctaPlusWidget>("OctaPlus");
