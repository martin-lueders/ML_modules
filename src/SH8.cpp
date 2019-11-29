#include "ML_modules.hpp"
#include "simd_mask.hpp"


using simd::float_4;


struct SH_channel {
	
	dsp::TSchmittTrigger<float_4> trigger[4];

	inline void step(int channels, const float_4 *in, const float_4 *trig, float_4 *out) {

		for(int c=0; c<channels; c+=4) {
			out[c/4] = ifelse(trigger[c/4].process(trig[c/4]), in[c/4], out[c/4]);
		}
	}

};

struct Random_Generator {

	__m128i x1, x2;
	simd::float_4 fscale;

	Random_Generator() {
		fscale = float_4(10.0f/0xffffffffu);
		x1 = _mm_set1_epi32(0x67452301);
		x2 = _mm_setr_epi32(0xefcdab89, 0xefcdcb89, 0xefcdab29, 0xefcdab81);
	};

	~Random_Generator() {};

	inline float_4 get() {
		x1 = _mm_xor_si128(x1, x2);
		x2 = _mm_add_epi32(x1, x2);
		return simd::int32_4(x2) * fscale;
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

	Random_Generator rand_gen;

	SH8() {
		config( NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS ); 
		onReset(); 
	};

	void process(const ProcessArgs &args) override;

	void onReset() override {
		memset(out,   0, sizeof(out));
	};

};

// Behavious depending on number of input channels:
//
//             input      1        N
//  trigger
//
//  1                     1        N
//  M                     M        N
//
//  monophonic input:
//  for M polyphonic trigger channels, we expect M output channels, where each represents the input at the given trigger time
//
//  polyphonic input (N channels)
//  for monophonic trigger, the output has N channels, triggered at the same time (given by mono trigger)
//  for polyphonic trigger (M channels): we have N output channels
//      N > M:  first M channels as expected, channels  N<i<M output of input channel i at last trigger of that channel
//      M > N:  first N channels as expected, triggers for i>N are ignored


void SH8::process(const ProcessArgs &args) {

	float_4 in[4];
	float_4 trig[4];

	memset(in,   0, sizeof(in));
	memset(trig, 0, sizeof(trig));

	int in_channels = 0;
	int trig_channels = 0;

	for(int c=0; c<PORT_MAX_CHANNELS; c+=4) in[c/4] = rand_gen.get();

	for(int i=0; i<8; i++) {

		int new_trig_channels = inputs[TRIG_INPUT+i].getChannels();
		int new_in_channels   = inputs[IN_INPUT  +i].getChannels();

		new_trig_channels = new_trig_channels==0?trig_channels:new_trig_channels;
		new_in_channels = new_in_channels==0?in_channels:new_in_channels;

		if( inputs[TRIG_INPUT+i].isConnected() ) {
			trig_channels = new_trig_channels==1?new_in_channels:new_trig_channels;
			for(int c=0; c<trig_channels; c+=4) {
				trig[c/4] = inputs[TRIG_INPUT].getPolyVoltageSimd<simd::float_4>(c);
			}
			channelMask.apply(trig, trig_channels==1?in_channels:trig_channels);
		}
		
		if(inputs[IN_INPUT+i].isConnected() ) {
			in_channels = new_in_channels==1?new_trig_channels:new_in_channels;
			for(int c=0; c<in_channels; c+=4) {
				in[c/4] = inputs[IN_INPUT].getPolyVoltageSimd<simd::float_4>(c);
			}
			channelMask.apply(in, in_channels==1?trig_channels:in_channels);
		}

		if(in_channels==1) in_channels = trig_channels;

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
