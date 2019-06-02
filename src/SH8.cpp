#include "ML_modules.hpp"


struct SH_channel {
	
	dsp::SchmittTrigger trigger[PORT_MAX_CHANNELS];

	void step(int channels, float *in, float *trig, float *out) {

		for(int c=0; c<channels; c++) {
			if( trigger[c].process(trig[c]) ) out[c] = in[c];
		}
	}

};

struct SH8 : Module {
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
		TRIG1_INPUT,
		TRIG2_INPUT,
		TRIG3_INPUT,
		TRIG4_INPUT,
		TRIG5_INPUT,
		TRIG6_INPUT,
		TRIG7_INPUT,
		TRIG8_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		OUT3_OUTPUT,
		OUT4_OUTPUT,
		OUT5_OUTPUT,
		OUT6_OUTPUT,
		OUT7_OUTPUT,
		OUT8_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	SH_channel sh_channel[8];

	float out[8*PORT_MAX_CHANNELS];


	SH8() {
		config( NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS ); 
		onReset(); 
	};

	void process(const ProcessArgs &args) override;

	void onReset() override {
		memset(out,   0, 8*PORT_MAX_CHANNELS * sizeof(float));
	};

};



void SH8::process(const ProcessArgs &args) {

	float in[  PORT_MAX_CHANNELS];
	float trig[PORT_MAX_CHANNELS];

	memset(in,   0, PORT_MAX_CHANNELS * sizeof(float));
	memset(trig, 0, PORT_MAX_CHANNELS * sizeof(float));

	// float random = 0;

	int in_channels = 0;
	// int trig_channels[8];

	if(inputs[IN1_INPUT].isConnected() ) {
		in_channels = inputs[IN1_INPUT].getChannels();
		inputs[IN1_INPUT].readVoltages(in);
	}

	if( inputs[TRIG1_INPUT].isConnected() ) {
	
		if (inputs[TRIG1_INPUT].getChannels()==1) {
			float val = inputs[TRIG1_INPUT].getVoltage();
			for(int c=0; c<in_channels; c++) trig[c] = val;
		} else {
			inputs[TRIG1_INPUT].readVoltages(trig);
		}

		sh_channel[0].step(in_channels, in, trig, out);

	}

	if( outputs[OUT1_OUTPUT].isConnected() ) {
		outputs[OUT1_OUTPUT].setChannels(in_channels);
		outputs[OUT1_OUTPUT].writeVoltages(out);
	}

	for(int i=1; i<8; i++) {

		if( inputs[TRIG1_INPUT+i].isConnected() ) {
			if ( inputs[TRIG1_INPUT+i].getChannels()==1 ) {
				float val = inputs[TRIG1_INPUT+i].getVoltage();
				for(int c=0; c < PORT_MAX_CHANNELS; c++) trig[c] = val;
			} else {
				inputs[TRIG1_INPUT+i].readVoltages(trig);
			}
		}
		
		if(inputs[IN1_INPUT+i].isConnected() ) {
			in_channels = inputs[IN1_INPUT+i].getChannels();
			inputs[IN1_INPUT+i].readVoltages(in);
		}

		sh_channel[i].step(in_channels, in, trig, out+i*PORT_MAX_CHANNELS );

		if( outputs[OUT1_OUTPUT+i].isConnected() ) {
			outputs[OUT1_OUTPUT+i].setChannels(in_channels);
			outputs[OUT1_OUTPUT+i].writeVoltages(out+i*PORT_MAX_CHANNELS);
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
		addInput(createInput<MLPort>(Vec(row1, offset_y + i*delta_y  ), module, SH8::IN1_INPUT+i));
		addInput(createInput<MLPort>(Vec(row2, offset_y + i*delta_y  ), module, SH8::TRIG1_INPUT+i));
		addOutput(createOutput<MLPort>(Vec(row3, offset_y + i*delta_y ), module, SH8::OUT1_OUTPUT+i));
	};


}

Model *modelSH8 = createModel<SH8, SH8Widget>("SH8");
