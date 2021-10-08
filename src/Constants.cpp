#include "ML_modules.hpp"


struct Constants : Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		NUM_INPUTS
	};
	enum OutputIds {
		P_1_OUTPUT,
		P_2_OUTPUT,
		P_3_OUTPUT,
		P_4_OUTPUT,
		P_5_OUTPUT,
		P_7_OUTPUT,
		P_12_OUTPUT,
		M_1_OUTPUT,
		M_2_OUTPUT,
		M_3_OUTPUT,
		M_4_OUTPUT,
		M_5_OUTPUT,
		M_7_OUTPUT,
		M_12_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};



	Constants() {

	config( NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS );

	configOutput(P_1_OUTPUT,  "+  1");
	configOutput(P_2_OUTPUT,  "+  2");
	configOutput(P_3_OUTPUT,  "+  3");
	configOutput(P_4_OUTPUT,  "+  4");
	configOutput(P_5_OUTPUT,  "+  5");
	configOutput(P_7_OUTPUT,  "+  7");
	configOutput(P_12_OUTPUT, "+ 12");
	configOutput(M_1_OUTPUT,  "-  1");
	configOutput(M_2_OUTPUT,  "-  2");
	configOutput(M_3_OUTPUT,  "-  3");
	configOutput(M_4_OUTPUT,  "-  4");
	configOutput(M_5_OUTPUT,  "-  5");
	configOutput(M_7_OUTPUT,  "-  7");
	configOutput(M_12_OUTPUT, "- 12");

	double semi = 1.0/12.0;

	for(int i=0; i<NUM_OUTPUTS; i++) outputs[i].setChannels(1);

	outputs[P_1_OUTPUT].setVoltage(semi);
	outputs[P_2_OUTPUT].setVoltage(2*semi);
	outputs[P_3_OUTPUT].setVoltage(3*semi);
	outputs[P_4_OUTPUT].setVoltage(4*semi);
	outputs[P_5_OUTPUT].setVoltage(5*semi);
	outputs[P_7_OUTPUT].setVoltage(7*semi);
	outputs[P_12_OUTPUT].setVoltage(1.0);

	outputs[M_1_OUTPUT].setVoltage(- semi  );
	outputs[M_2_OUTPUT].setVoltage(- 2*semi);
	outputs[M_3_OUTPUT].setVoltage(- 3*semi);
	outputs[M_4_OUTPUT].setVoltage(- 4*semi);
	outputs[M_5_OUTPUT].setVoltage(- 5*semi);
	outputs[M_7_OUTPUT].setVoltage(- 7*semi);
	outputs[M_12_OUTPUT].setVoltage(- 1.0);

	};

	void process(const ProcessArgs &args) override;

};



void Constants::process(const ProcessArgs &args) {


	double semi = 1.0/12.0;

	for(int i=0; i<NUM_OUTPUTS; i++) outputs[i].setChannels(1);

	outputs[P_1_OUTPUT].setVoltage(semi);
	outputs[P_2_OUTPUT].setVoltage(2*semi);
	outputs[P_3_OUTPUT].setVoltage(3*semi);
	outputs[P_4_OUTPUT].setVoltage(4*semi);
	outputs[P_5_OUTPUT].setVoltage(5*semi);
	outputs[P_7_OUTPUT].setVoltage(7*semi);
	outputs[P_12_OUTPUT].setVoltage(1.0);

	outputs[M_1_OUTPUT].setVoltage(- semi  );
	outputs[M_2_OUTPUT].setVoltage(- 2*semi);
	outputs[M_3_OUTPUT].setVoltage(- 3*semi);
	outputs[M_4_OUTPUT].setVoltage(- 4*semi);
	outputs[M_5_OUTPUT].setVoltage(- 5*semi);
	outputs[M_7_OUTPUT].setVoltage(- 7*semi);
	outputs[M_12_OUTPUT].setVoltage(- 1.0);
	

};



struct ConstantsWidget : ModuleWidget {
	ConstantsWidget(Constants *module);
};

ConstantsWidget::ConstantsWidget(Constants *module) {
		setModule(module);

	box.size = Vec(15*6, 380);

	{
		SvgPanel *panel = new SvgPanel();
		panel->box.size = box.size;
		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance,"res/Constants.svg")));

		addChild(panel);
	}

	addChild(createWidget<MLScrew>(Vec(15, 0)));
	addChild(createWidget<MLScrew>(Vec(15, 365)));




	const float offset_y = 63, delta_y = 40, offset_xL=10, offset_xR=55;


	for(int i=0; i<7; i++) {

		addOutput(createOutput<MLPort>(Vec(offset_xR, offset_y + i*delta_y), module, Constants::P_1_OUTPUT + i));
		addOutput(createOutput<MLPort>(Vec(offset_xL, offset_y + i*delta_y), module, Constants::M_1_OUTPUT + i));

	};


}

Model *modelConstants = createModel<Constants, ConstantsWidget>("Constants");
