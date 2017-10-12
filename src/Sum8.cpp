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

#ifdef v032
	Sum8() ;
#endif
#ifdef v040
	Sum8() : Module( NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS ) {};
#endif


	void step();

};

#ifdef v032
Sum8::Sum8() {
	params.resize(NUM_PARAMS);
	inputs.resize(NUM_INPUTS);
	outputs.resize(NUM_OUTPUTS);
};
#endif



void Sum8::step() {

	float out=0.0;

#ifdef v032

	for(int i=0; i<8; i++) {

		if( inputs[IN1_INPUT+i] ) out +	getf(inputs[IN1_INPUT+i]);

	};

	setf(outputs[OUT_OUTPUT],out);
#endif


#ifdef v040

	for(int i=0; i<8; i++) out += inputs[IN1_INPUT+i].normalize(0.0);

	outputs[OUT_OUTPUT].value = out;
#endif

};



Sum8Widget::Sum8Widget() {

	Sum8 *module = new Sum8();
	setModule(module);
	box.size = Vec(15*3, 380);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
#ifdef v032
		panel->setBackground(SVG::load("plugins/ML_modules/res/Sum8.svg"));
#endif
#ifdef v040
		panel->setBackground(SVG::load(assetPlugin(plugin,"res/Sum8.svg")));
#endif

		addChild(panel);
	}

	addChild(createScrew<ScrewSilver>(Vec(15, 0)));
	addChild(createScrew<ScrewSilver>(Vec(15, 365)));




	const float offset_y = 70, delta_y = 26, offset_x=10.5;

	for( int i=0; i<8; i++) addInput(createInput<PJ301MPort>(Vec(offset_x, offset_y + i*delta_y  ),    module, Sum8::IN1_INPUT+i));


	addOutput(createOutput<PJ301MPort>(Vec(offset_x, 320), module, Sum8::OUT_OUTPUT));


}
