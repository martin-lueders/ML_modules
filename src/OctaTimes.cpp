#include "ML_modules.hpp"


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

	dsp::SchmittTrigger trigger[8];
	float out[8];



	OctaTimes() {
		config( NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS ) ;
	    configParam(OctaTimes::MULT_PARAM , 0.0, 1.0, 0.0);
		onReset();
	};

	void process(const ProcessArgs &args) override;

	void onReset() override {
		for(int i=0; i<8; i++) out[i] = 0.0; 
	};

};


/*
void OctaTimes::process(const ProcessArgs &args) {

	float in_A[8], in_B[8];

	float normal = params[MULT_PARAM].getValue()==1?1.0f:10.f;


	float multiplier = params[MULT_PARAM].getValue()==1?1.f:0.1f;

	in_A[0] = inputs[IN1_INPUT].getNormalVoltage(0.f);
//	for(int i=1; i<8; i++) in_A[i] = inputs[IN1_INPUT+i].normalize(in_A[i-1]);
	for(int i=1; i<8; i++) in_A[i] = inputs[IN1_INPUT+i].getNormalVoltage(0.f);

	in_B[0] = inputs[IN_B_1_INPUT].getNormalVoltage(normal);
	for(int i=1; i<8; i++) in_B[i] = inputs[IN_B_1_INPUT+i].getNormalVoltage(in_B[i-1]);

	float tmp;
	float sum = 0.0f;
	for(int i=0; i<8; i++) {
		tmp = clamp(in_A[i] * in_B[i] * multiplier , -12.f, 12.f);
		outputs[OUT1_OUTPUT+i].setVoltage(tmp);
		sum += tmp;
	};

	outputs[SUM_OUTPUT].setVoltage(clamp(sum,-12.f,12.f));
};
*/

void OctaTimes::process(const ProcessArgs &args) {

	float in_A[PORT_MAX_CHANNELS], in_B[PORT_MAX_CHANNELS], out[PORT_MAX_CHANNELS], sum[PORT_MAX_CHANNELS];

	int channels_A = 0, channels_B = 0, channels_OUT = 0;

	float multiplier = params[MULT_PARAM].getValue()==1?1.f:0.1f;

	memset(in_A, 0, PORT_MAX_CHANNELS*sizeof(float));
	if(params[MULT_PARAM].getValue()==1) {
		for(int c=0; c < PORT_MAX_CHANNELS; c++) in_B[c] = 1.0f;
	} else {
		for(int c=0; c < PORT_MAX_CHANNELS; c++) in_B[c] = 10.0f;
	}
	memset(out,  0, PORT_MAX_CHANNELS*sizeof(float));
	memset(sum,  0, PORT_MAX_CHANNELS*sizeof(float));

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

		for(int c=0; c<channels_OUT; c++) {
			float tmp = clamp(in_A[c] * in_B[c] * multiplier, -12.f, 12.f );
			out[c] = tmp;
			sum[c] += tmp;
		}
		outputs[OUT_OUTPUT+i].setChannels(channels_OUT);
		outputs[OUT_OUTPUT+i].writeVoltages(out);
		outputs[SUM_OUTPUT].writeVoltages(sum);
	
	};

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
		addOutput(createOutput<MLPort>(Vec(row3, offset_y + i*delta_y ), module, OctaTimes::OUT_OUTPUT+i));
	};
	
	addOutput(createOutput<MLPort>(Vec(row3, 330 ), module, OctaTimes::SUM_OUTPUT));
	addParam(createParam<CKSS>( Vec(row1 + 5, 330 ), module, OctaTimes::MULT_PARAM ));
}

Model *modelOctaTimes = createModel<OctaTimes, OctaTimesWidget>("OctaTimes");
