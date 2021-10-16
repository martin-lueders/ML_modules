#include "ML_modules.hpp"
#include "simd_mask.hpp"

using simd::float_4;

struct OctaTimes : Module {
	enum ParamIds {
		MULT_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		IN_A_INPUT,
		IN_B_INPUT = IN_A_INPUT + 8,
		NUM_INPUTS = IN_B_INPUT + 8
	};
	enum OutputIds {
		OUT_OUTPUT,
		SUM_OUTPUT = OUT_OUTPUT + 8,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	float out[8];

	ChannelMask channelMask;

	OctaTimes() {
		config( NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS ) ;
	    configSwitch(OctaTimes::MULT_PARAM , 0.0, 1.0, 0.0, "Scale", {"x1", "/10"});
		for(int i=0; i<8; i++) {
			configInput(IN_A_INPUT+i, "A #"+std::to_string(i+1));
			configInput(IN_B_INPUT+i, "B #"+std::to_string(i+1));
			configOutput(OUT_OUTPUT+i, "#"+std::to_string(i+1));
		}
		configOutput(SUM_OUTPUT, "Sum");
	};

	void process(const ProcessArgs &args) override;

};



void OctaTimes::process(const ProcessArgs &args) {

	float_4 in_A[4], in_B[4], out[4], sum[4];

	float_4 normal     = float_4(params[MULT_PARAM].getValue()==1?1.0f:10.f);
	float_4 multiplier = float_4(params[MULT_PARAM].getValue()==1?1.f:0.1f);

	int channels_A = 0, channels_B = 0, channels_OUT = 0, channels_SUM = 0;

	memset(in_A, 0, sizeof(in_A));
	memset(out,  0, sizeof(out));
	memset(sum,  0, sizeof(sum));

	for(int c=0; c<4; c++) in_B[c] = normal;

	for(int i=0; i<8; i++) {

		int tmp_A = inputs[IN_A_INPUT+i].getChannels();
		int tmp_B = inputs[IN_B_INPUT+i].getChannels();

		channels_A = tmp_A>0 ? tmp_A : channels_A; 
		channels_B = tmp_B>0 ? tmp_B : channels_B; 

		load_input(inputs[IN_A_INPUT+i], in_A, tmp_A);
		load_input(inputs[IN_B_INPUT+i], in_B, tmp_B);

		channels_OUT = MAX(channels_A, channels_B);
		channels_SUM = MAX(channels_OUT, channels_SUM);

		for(int c=0; c<channels_OUT; c+=4) {
			out[c/4] = clamp( in_A[c/4] * in_B[c/4] * multiplier, float_4(-12.0f), float_4(12.f) );
			if(tmp_A+tmp_B>0) sum[c/4] += out[c/4];
		}
		outputs[OUT_OUTPUT+i].setChannels(channels_OUT);
		for(int c=0; c<channels_OUT; c+=4) out[c/4].store(outputs[OUT_OUTPUT+i].getVoltages(c));	
	};
	outputs[SUM_OUTPUT].setChannels(channels_SUM);
	for(int c=0; c<channels_SUM; c+=4) sum[c/4].store(outputs[SUM_OUTPUT].getVoltages(c));	
};



struct OctaTimesWidget : ModuleWidget {
	OctaTimesWidget(OctaTimes *module);
};

OctaTimesWidget::OctaTimesWidget(OctaTimes *module) {
		setModule(module);

	box.size = Vec(15*8, 380);

	{
		SvgPanel *panel = new SvgPanel();
		panel->box.size = box.size;
		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance,"res/OctaTimes.svg")));

		addChild(panel);
	}

	addChild(createWidget<MLScrew>(Vec(15, 0)));
	addChild(createWidget<MLScrew>(Vec(box.size.x-30, 0)));
	addChild(createWidget<MLScrew>(Vec(15, 365)));
	addChild(createWidget<MLScrew>(Vec(box.size.x-30, 365)));




	const float offset_y = 60, delta_y = 32, row1=15, row2 = 48, row3 = 80;

	for( int i=0; i<8; i++) {
		addInput(createInput<MLPort>(Vec(row1, offset_y + i*delta_y  ), module, OctaTimes::IN_A_INPUT+i));
		addInput(createInput<MLPort>(Vec(row2, offset_y + i*delta_y  ), module, OctaTimes::IN_B_INPUT+i));
		addOutput(createOutput<MLPortOut>(Vec(row3, offset_y + i*delta_y ), module, OctaTimes::OUT_OUTPUT+i));
	};
	
	addOutput(createOutput<MLPortOut>(Vec(row3, 330 ), module, OctaTimes::SUM_OUTPUT));
	addParam(createParam<CKSS>( Vec(row1 + 5, 330 ), module, OctaTimes::MULT_PARAM ));
}

Model *modelOctaTimes = createModel<OctaTimes, OctaTimesWidget>("OctaTimes");
