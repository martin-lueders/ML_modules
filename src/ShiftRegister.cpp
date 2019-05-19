#include "ML_modules.hpp"


struct ShiftRegister : Module {
	enum ParamIds {
		NUM_STEPS,
		STEP1_PARAM,
		NUM_PARAMS = STEP1_PARAM+8
	};
	enum InputIds {
		IN_INPUT,
		TRIGGER_INPUT,
		TRIGUP_INPUT,
		TRIGDN_INPUT,
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
		// OUT_OUTPUT,
		NUM_OUTPUTS
	};
        enum LightIds {
                STEP1_LIGHT,
                STEP2_LIGHT,
                STEP3_LIGHT,
                STEP4_LIGHT,
                STEP5_LIGHT,
                STEP6_LIGHT,
                STEP7_LIGHT,
                STEP8_LIGHT,
                NUM_LIGHTS = STEP1_LIGHT+16
        };


	ShiftRegister() : Module( NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS ) { reset(); };



	void process(const ProcessArgs &args) override;

	int position=0;


	float values[8] = {};

	dsp::SchmittTrigger upTrigger, downTrigger, setTrigger;

	void reset() override {
		position=0;
		for(int i=0; i<8; i++) {
			lights[i].value = 0.0;
			values[i] = 0.0;
		};
	};


};




void ShiftRegister::process(const ProcessArgs &args) {

	if( inputs[TRIGGER_INPUT].isConnected() ) {

		if( setTrigger.process(inputs[TRIGGER_INPUT].getVoltage()) ) {

			for(int i=7; i>0; i--) values[i] = values[i-1];
			values[0] = inputs[IN_INPUT].getVoltage();

			for(int i=0; i<8; i++) {
				outputs[OUT1_OUTPUT+i].setVoltage(values[i]);
				bool positive = values[i]>0;
				float value = values[i]/10.0;
				lights[STEP1_LIGHT+2*i].value   = positive?value:0.0;
				lights[STEP1_LIGHT+2*i+1].value = -(positive?0.0:value);
			}
		};

	};


};



struct ShiftRegisterWidget : ModuleWidget {
	ShiftRegisterWidget(ShiftRegister *module);
};

ShiftRegisterWidget::ShiftRegisterWidget(ShiftRegister *module) {
		setModule(module);

	box.size = Vec(15*4, 380);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance,"res/ShiftReg.svg")));

		addChild(panel);
	}

	addChild(createWidget<MLScrew>(Vec(15, 0)));
	addChild(createWidget<MLScrew>(Vec(15, 365)));




	const float offset_y = 140, delta_y = 26, offset_x=11;

	for( int i=0; i<8; i++) {

		addOutput(createOutput<MLPort>(Vec(offset_x+17, offset_y + i*delta_y  ), module, ShiftRegister::OUT1_OUTPUT+i));
		addChild(createLight<MLSmallLight<GreenRedLight>>(Vec(offset_x, offset_y + 8 +   i*delta_y), module, ShiftRegister::STEP1_LIGHT+2*i));
	};


	addInput(createInput<MLPort>(Vec(offset_x+17, 58), module, ShiftRegister::IN_INPUT));
	addInput(createInput<MLPort>(Vec(offset_x+17, 94), module, ShiftRegister::TRIGGER_INPUT));

}

Model *modelShiftRegister = createModel<ShiftRegister, ShiftRegisterWidget>("ShiftRegister");