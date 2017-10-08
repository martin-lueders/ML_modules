#include "ML_modules.hpp"


struct TrigBuf : Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		ARM1_INPUT,
		ARM2_INPUT,
		TRIG1_INPUT,
		TRIG2_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		NUM_OUTPUTS
	};

#ifdef v040
	TrigBuf() : Module( NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS ) {};
#endif

#ifdef v032	
	TrigBuf() ;
#endif

	void step();

	float arm1=0.0, arm2=0.0;
	float out1=0.0, out2=0.0;

	bool fired1=false, fired2=false;

	SchmittTrigger armTrigger1, armTrigger2;
	SchmittTrigger trigTrigger1, trigTrigger2;

	void initialize(){
		arm1=0.0;
		arm2=0.0;
		out1=0.0;
		out2=0.0;
	};

};

#ifdef v032
TrigBuf::TrigBuf() {
	params.resize(NUM_PARAMS);
	inputs.resize(NUM_INPUTS);
	outputs.resize(NUM_OUTPUTS);
};
#endif



void TrigBuf::step() {


	bool trig1 = false;
	bool trig2 = false;
       

#ifdef v032
	if( inputs[ARM1_INPUT] ) {
		if( armTrigger1.process(*inputs[ARM1_INPUT]) ) { 
			arm1 = 10.0;
			if ( !inputs[ARM2_INPUT] ) arm2 = 10.0;
	       	}
	} else {
		arm1 = 0.0;
	};
#endif

#ifdef v040
	if( inputs[ARM1_INPUT].active ) {
		if( armTrigger1.process(inputs[ARM1_INPUT].value) ) { 
			arm1 = 10.0;
			if ( !inputs[ARM2_INPUT].active ) arm2 = 10.0;
	       	}
	} else {
		arm1 = 0.0;
	};
#endif



#ifdef v032
	if( inputs[TRIG1_INPUT]) trigTrigger1.process(*inputs[TRIG1_INPUT]);
	trig = (trigTrigger1:state == SchmittTrigger::HIGH);
#endif
#ifdef v040
	if( inputs[TRIG1_INPUT].active) trigTrigger1.process(inputs[TRIG1_INPUT].value);
	trig1 = (trigTrigger1.isHigh());
#endif


        if(trig1) {
		if(arm1 > 5.0) {
			out1 = 10.0;
		} else {
			out1 = 0.0;
		};
	}
	else {
		if(out1 > 5.0) {
			arm1 = 0.0;
			out1 = 0.0;
		};
	};



#ifdef v032
	if( inputs[ARM2_INPUT] ) {
		if( armTrigger2.process(*inputs[ARM2_INPUT]) ) { arm2 = 10.0; }
	};
#endif


#ifdef v040
	if( inputs[ARM2_INPUT].active ) {
		if( armTrigger2.process(inputs[ARM2_INPUT].value) ) { arm2 = 10.0; }
	};
#endif

#ifdef v032
	if( inputs[TRIG2_INPUT] ) {
		trigTrigger2.process(*inputs[TRIG2_INPUT]) ; 
		trig2 = (trigTrigger2.isHigh())
	} else {
		trig2 = trig1;
	};
#endif

#ifdef v040
	if( inputs[TRIG2_INPUT].active ) {
		trigTrigger2.process(inputs[TRIG2_INPUT].value);
		trig2 = trigTrigger2.isHigh();
	} else {
		trig2 = trig1;
	}
#endif			

	if (trig2) {

		if(arm2 > 5.0) out2 = 10.0;
		else {
			out2 = 0.0;
		};
	} else {
		if(out2 > 5.0) {
			arm2 = 0.0;
			out2 = 0.0;
		};
	};

#ifdef v032
	setf(outputs[OUT1_OUTPUT],out1);
	setf(outputs[OUT2_OUTPUT],out2);
#endif

#ifdef v040
	outputs[OUT1_OUTPUT].value = out1;
	outputs[OUT2_OUTPUT].value = out2;
#endif

};



TrigBufWidget::TrigBufWidget() {

	TrigBuf *module = new TrigBuf();
	setModule(module);
	box.size = Vec(15*4, 380);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
#ifdef v032		
		panel->setBackground(SVG::load("plugins/ML_modules/res/TrigBuf.svg"));
#endif
#ifdef v040
		panel->setBackground(SVG::load(assetPlugin(plugin,"res/TrigBuf.svg")));
#endif
		addChild(panel);
	}

	addChild(createScrew<ScrewSilver>(Vec(15, 0)));
	addChild(createScrew<ScrewSilver>(Vec(15, 365)));

	addInput(createInput<PJ301MPort>(Vec(10, 62),    module, TrigBuf::ARM1_INPUT));
	addInput(createInput<PJ301MPort>(Vec(10, 105),    module, TrigBuf::TRIG1_INPUT));
	addOutput(createOutput<PJ301MPort>(Vec(10, 150), module, TrigBuf::OUT1_OUTPUT));
	addChild(createValueLight<SmallLight<GreenValueLight>>(Vec(46, 66), &module->arm1));

	addInput(createInput<PJ301MPort>(Vec(10, 218),   module, TrigBuf::ARM2_INPUT));
	addInput(createInput<PJ301MPort>(Vec(10, 263),   module, TrigBuf::TRIG2_INPUT));
	addOutput(createOutput<PJ301MPort>(Vec(10, 305), module, TrigBuf::OUT2_OUTPUT));
	addChild(createValueLight<SmallLight<GreenValueLight>>(Vec(46, 222), &module->arm2));
}
