#include "ML_modules.hpp"
#include "simd_mask.hpp"
#include "PulseGenerator_4.hpp"


using simd::float_4;


struct OctaTrig : Module {
	enum ParamIds {
		RESET_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		ENUMS(IN_INPUT, 8),
		NUM_INPUTS
	};
	enum OutputIds {
		ENUMS(UP_OUTPUT, 8),
		ENUMS(DN_OUTPUT, 8),
		ENUMS(SUM_OUTPUT, 8),
		NUM_OUTPUTS
	};

	enum LightIds {
		NUM_LIGHTS
	};

	PulseGenerator_4 upPulse[8][4], dnPulse[8][4];
	ChannelMask channelMask;

	float_4 state[8][4];

	OctaTrig() {
		config( NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS );

		for(int i=0; i<8; i++) {
			configInput(IN_INPUT+i, "Gate #"+std::to_string(i+1));
			configOutput(UP_OUTPUT+i, "Trigger up #"+std::to_string(i+1));
			configOutput(DN_OUTPUT+i, "Trigger down #"+std::to_string(i+1));
			configOutput(SUM_OUTPUT+i, "Trigger both #"+std::to_string(i+1));
		}

		memset(state, 0, sizeof(state));
	};

	void process(const ProcessArgs &args) override;
};



void OctaTrig::process(const ProcessArgs &args) {

	float delta = args.sampleTime;

	for(int i=0; i<8; i++) {
	
		float_4 gate[4];

		int channels = inputs[IN_INPUT+i].getChannels();

		if(channels>0) {

			load_input(inputs[IN_INPUT+i], gate, channels);

			outputs[UP_OUTPUT +i].setChannels(channels);
			outputs[DN_OUTPUT +i].setChannels(channels);
			outputs[SUM_OUTPUT+i].setChannels(channels);
		
			for(int c=0; c < channels; c+=4) {

				float_4 gate_mask   = gate[c/4]>float_4(1.0f);
				float_4 change_mask = gate_mask^state[i][c/4];

				upPulse[i][c/4].trigger(change_mask &   gate_mask,  0.001f);
				dnPulse[i][c/4].trigger(change_mask & (~gate_mask), 0.001f);
			
				float_4 out_up = ifelse( upPulse[i][c/4].process(delta), float_4(10.0f), float_4::zero() );
				float_4 out_dn = ifelse( dnPulse[i][c/4].process(delta), float_4(10.0f), float_4::zero() );

				float_4 out_sum = clamp(out_up + out_dn, float_4::zero(), float_4(10.f));

				out_up.store( outputs[UP_OUTPUT +i].getVoltages(c));	
				out_dn.store( outputs[DN_OUTPUT +i].getVoltages(c));
				out_sum.store(outputs[SUM_OUTPUT+i].getVoltages(c));

				state[i][c/4] = gate_mask;

			} // c
		} // channels > 0
	} // i
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
		addInput(createInput<MLPort>(  Vec(row1, offset_y + i*delta_y ), module, OctaTrig::IN_INPUT+i));

		addOutput(createOutput<MLPort>(Vec(row2, offset_y + i*delta_y ), module, OctaTrig::UP_OUTPUT+i));
		addOutput(createOutput<MLPort>(Vec(row3, offset_y + i*delta_y ), module, OctaTrig::DN_OUTPUT+i));
		addOutput(createOutput<MLPort>(Vec(row4, offset_y + i*delta_y ), module, OctaTrig::SUM_OUTPUT+i));
	};


}

Model *modelOctaTrig = createModel<OctaTrig, OctaTrigWidget>("OctaTrig");
