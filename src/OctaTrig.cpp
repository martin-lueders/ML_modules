#include "ML_modules.hpp"


struct OctaTrig : Module {
	enum ParamIds {
		RESET_PARAM,
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
		UP1_OUTPUT,
		UP2_OUTPUT,
		UP3_OUTPUT,
		UP4_OUTPUT,
		UP5_OUTPUT,
		UP6_OUTPUT,
		UP7_OUTPUT,
		UP8_OUTPUT,
		DN1_OUTPUT,
		DN2_OUTPUT,
		DN3_OUTPUT,
		DN4_OUTPUT,
		DN5_OUTPUT,
		DN6_OUTPUT,
		DN7_OUTPUT,
		DN8_OUTPUT,
		SUM1_OUTPUT,
		SUM2_OUTPUT,
		SUM3_OUTPUT,
		SUM4_OUTPUT,
		SUM5_OUTPUT,
		SUM6_OUTPUT,
		SUM7_OUTPUT,
		SUM8_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightIds {
		NUM_LIGHTS
	};

	dsp::PulseGenerator upPulse[8 * PORT_MAX_CHANNELS], dnPulse[8 * PORT_MAX_CHANNELS];

	bool state[8 * PORT_MAX_CHANNELS] = {};


	OctaTrig() {

		config( NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS ) ;
		for(int i=0; i<8 * PORT_MAX_CHANNELS; i++) state[i]=false; 
	};

	void process(const ProcessArgs &args) override;

};



void OctaTrig::process(const ProcessArgs &args) {

	bool gate[PORT_MAX_CHANNELS];

	float out_up[PORT_MAX_CHANNELS] = {};
	float out_dn[PORT_MAX_CHANNELS] = {};
	float out_sum[PORT_MAX_CHANNELS] = {};


	float delta = args.sampleTime;

	for(int i=0; i<8; i++) {
		int channels = inputs[IN1_INPUT+i].getChannels();
		
		for(int c=0; c < channels; c++) {

			gate[c] = inputs[IN1_INPUT+i].getNormalPolyVoltage(0.0, c) > 1.0;

			if (gate[c]^state[i*PORT_MAX_CHANNELS + c]) {

				if(gate[c]) upPulse[i*PORT_MAX_CHANNELS + c].trigger(0.01);
				else	    dnPulse[i*PORT_MAX_CHANNELS + c].trigger(0.01);
			}

			out_up[c] = upPulse[i*PORT_MAX_CHANNELS + c].process(delta)?10.0:0.0;
			out_dn[c] = dnPulse[i*PORT_MAX_CHANNELS + c].process(delta)?10.0:0.0;

			out_sum[c] = out_up[i*PORT_MAX_CHANNELS + c] + out_dn[i*PORT_MAX_CHANNELS + c];

			state[i*PORT_MAX_CHANNELS + c] = gate[i];

		}

		outputs[UP1_OUTPUT+i ].setChannels(channels);
		outputs[DN1_OUTPUT+i ].setChannels(channels);
		outputs[SUM1_OUTPUT+i].setChannels(channels);



		outputs[UP1_OUTPUT+i].writeVoltages(out_up);
		outputs[DN1_OUTPUT+i].writeVoltages(out_dn);
		outputs[SUM1_OUTPUT+i].writeVoltages(out_sum);
	}

};



struct OctaTrigWidget : ModuleWidget {
	OctaTrigWidget(OctaTrig *module);
};

OctaTrigWidget::OctaTrigWidget(OctaTrig *module) {
		setModule(module);

	box.size = Vec(15*10, 380);

	{
		SvgPanel *panel = new SvgPanel();
		panel->box.size = box.size;
		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance,"res/OctaTrig.svg")));

		addChild(panel);
	}

	addChild(createWidget<MLScrew>(Vec(15, 0)));
	addChild(createWidget<MLScrew>(Vec(box.size.x-30, 0)));
	addChild(createWidget<MLScrew>(Vec(15, 365)));
	addChild(createWidget<MLScrew>(Vec(box.size.x-30, 365)));



	const float offset_y = 60, delta_y = 32, row1=15, row2 = 50, row3 = 80, row4 = 110;

	for( int i=0; i<8; i++) {
		addInput(createInput<MLPort>(Vec(row1, offset_y + i*delta_y  ), module, OctaTrig::IN1_INPUT+i));

		addOutput(createOutput<MLPort>(Vec(row2, offset_y + i*delta_y ), module, OctaTrig::UP1_OUTPUT+i));
		addOutput(createOutput<MLPort>(Vec(row3, offset_y + i*delta_y ), module, OctaTrig::DN1_OUTPUT+i));
		addOutput(createOutput<MLPort>(Vec(row4, offset_y + i*delta_y ), module, OctaTrig::SUM1_OUTPUT+i));
	};


}

Model *modelOctaTrig = createModel<OctaTrig, OctaTrigWidget>("OctaTrig");
