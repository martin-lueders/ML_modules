#include "ML_modules.hpp"
#include "simd_mask.hpp"


using simd::float_4;


struct SH_channel {
	
	dsp::TSchmittTrigger<float_4> trigger[4];

	inline void step(int channels, float_4 *in, float_4 *trig, float_4 *out) {

		for(int c=0; c<channels; c+=4) {
			out[c/4] = ifelse(trigger[c/4].process(trig[c/4]), in[c/4], out[c/4]);
		}
	}

};

struct SH8 : Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		IN_INPUT,
		TRIG_INPUT = IN_INPUT + 8,
		NUM_INPUTS = TRIG_INPUT + 8
	};
	enum OutputIds {
		OUT_OUTPUT,
		NUM_OUTPUTS = OUT_OUTPUT + 8
	};
	enum LightIds {
		NUM_LIGHTS
	};

	SH_channel sh_channel[8];

	float_4 out[8][4];
	ChannelMask channelMask;

	SH8() {
		config( NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS ); 
		onReset(); 
	};

	void process(const ProcessArgs &args) override;

	void onReset() override {
		memset(out,   0, sizeof(out));
	};

};



void SH8::process(const ProcessArgs &args) {

	float_4 in[4];
	float_4 trig[4];

	memset(in,   0, sizeof(in));
	memset(trig, 0, sizeof(trig));

	int in_channels = 0;
	int trig_channels = 0;

	for(int i=0; i<8; i++) {

		int new_trig_channels = inputs[TRIG_INPUT+i].getChannels();
		int new_in_channels = inputs[IN_INPUT+i].getChannels();

		if( inputs[TRIG_INPUT+i].isConnected() ) {
			trig_channels = new_trig_channels;
			load_input(inputs[TRIG_INPUT+i], trig, trig_channels);
			channelMask.apply(trig, trig_channels==1?in_channels:trig_channels);
		}
		
		if(inputs[IN_INPUT+i].isConnected() ) {
			in_channels = new_in_channels;
			load_input(inputs[IN_INPUT+i], in, in_channels);
			channelMask.apply(in, in_channels==1?trig_channels:in_channels);
		}

		sh_channel[i].step(in_channels, in, trig, out[i] );

		if( outputs[OUT_OUTPUT+i].isConnected() ) {
			outputs[OUT_OUTPUT+i].setChannels(std::max(in_channels,trig_channels));
			for(int c=0; c<in_channels; c+=4) out[i][c/4].store(outputs[OUT_OUTPUT+i].getVoltages(c));
		}

	}

};


struct SH8Widget : ModuleWidget {
	SH8Widget(SH8 *module);
};

SH8Widget::SH8Widget(SH8 *module) {

	setModule(module);
	box.size = Vec(15*8, 380);

	{
		SvgPanel *panel = new SvgPanel();
		panel->box.size = box.size;
		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance,"res/SH8.svg")));

		addChild(panel);
	}

	addChild(createWidget<MLScrew>(Vec(15, 0)));
	addChild(createWidget<MLScrew>(Vec(box.size.x-30, 0)));
	addChild(createWidget<MLScrew>(Vec(15, 365)));
	addChild(createWidget<MLScrew>(Vec(box.size.x-30, 365)));

	const float offset_y = 60, delta_y = 32, row1=15, row2 = 48, row3 = 80;

	for( int i=0; i<8; i++) {
		addInput(createInput<MLPort>(  Vec(row1, offset_y + i*delta_y ), module, SH8::IN_INPUT+i));
		addInput(createInput<MLPort>(  Vec(row2, offset_y + i*delta_y ), module, SH8::TRIG_INPUT+i));
		addOutput(createOutput<MLPort>(Vec(row3, offset_y + i*delta_y ), module, SH8::OUT_OUTPUT+i));
	};

}

Model *modelSH8 = createModel<SH8, SH8Widget>("SH8");
