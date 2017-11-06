#include "ML_modules.hpp"


struct Sum8 : Module {
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
		NUM_INPUTS
	};
	enum OutputIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS 
	};

#ifdef v040
	Sum8() : Module( NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS ) {};
#endif

#ifdef v_dev
	Sum8() : Module( NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS ) {};
#endif


	void step() override;

};



void Sum8::step() {

	float out=0.0;



	for(int i=0; i<8; i++) out += inputs[IN1_INPUT+i].normalize(0.0);

	outputs[OUT_OUTPUT].value = out;

};



Sum8Widget::Sum8Widget() {

	Sum8 *module = new Sum8();
	setModule(module);
	box.size = Vec(15*3, 380);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin,"res/Sum8.svg")));

		addChild(panel);
	}

	addChild(createScrew<ScrewSilver>(Vec(15, 0)));
	addChild(createScrew<ScrewSilver>(Vec(15, 365)));




	const float offset_y = 70, delta_y = 26, offset_x=10.5;

	for( int i=0; i<8; i++) addInput(createInput<PJ301MPort>(Vec(offset_x, offset_y + i*delta_y  ),    module, Sum8::IN1_INPUT+i));


	addOutput(createOutput<PJ301MPort>(Vec(offset_x, 320), module, Sum8::OUT_OUTPUT));


}
