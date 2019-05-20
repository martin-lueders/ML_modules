#include "ML_modules.hpp"

struct Quantum : Module {
	enum ParamIds {
		SEMI_1_PARAM,
		SEMI_2_PARAM,
		SEMI_3_PARAM,
		SEMI_4_PARAM,
		SEMI_5_PARAM,
		SEMI_6_PARAM,
		SEMI_7_PARAM,
		SEMI_8_PARAM,
		SEMI_9_PARAM,
		SEMI_10_PARAM,
		SEMI_11_PARAM,
		SEMI_12_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		IN_INPUT,
		TRANSPOSE_INPUT,
		NOTE_INPUT,
		SET_INPUT,
		RESET_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT_OUTPUT,
		TRIGGER_OUTPUT,
		GATE_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		SEMI_1_LIGHT,
		SEMI_2_LIGHT,
		SEMI_3_LIGHT,
		SEMI_4_LIGHT,
		SEMI_5_LIGHT,
		SEMI_6_LIGHT,
		SEMI_7_LIGHT,
		SEMI_8_LIGHT,
		SEMI_9_LIGHT,
		SEMI_10_LIGHT,
		SEMI_11_LIGHT,
		SEMI_12_LIGHT,
		NUM_LIGHTS
	};

	enum Mode {
		LAST,
		CLOSEST_UP,
		CLOSEST_DOWN,
		UP,
		DOWN
	};



	Mode mode = CLOSEST_UP;

	bool transpose_select = true;

	Quantum() {
		config( NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS); 
		for(int i=0; i<12; i++) {
		    configParam(Quantum::SEMI_1_PARAM + i, 0.0, 1.0, 0.0);
		}
		onReset(); 
	};

	void process(const ProcessArgs &args) override;
	dsp::PulseGenerator pulse;

	int last_octave=0, last_semi=0;

	bool semiState[12] = {};
	dsp::SchmittTrigger semiTriggers[12], setTrigger, resetTrigger;
	float semiLight[12] = {};

    void onReset() override {
        for (int i = 0; i < 12; i++) {
            semiState[i] = false;
			semiLight[i] = 0.0;
        }
		mode = CLOSEST_UP;
		last_octave = 0;
		last_semi   = 0;
	}

	void onRandomize() override {
		for (int i = 0; i<12; i++) {
			semiState[i] = (random::uniform() > 0.5);
			semiLight[i] = semiState[i]?1.0:0.0;
		};
		last_octave = 0;
		last_semi   = 0;

	}

    json_t *dataToJson() override {
        json_t *rootJ = json_object();

        json_t *scaleJ = json_array();
            for (int i = 0; i < 12; i++) {
                    json_t *semiJ = json_integer( (int) semiState[i]);
                    json_array_append_new(scaleJ, semiJ);
            }
            json_object_set_new(rootJ, "scale", scaleJ);
            json_object_set_new(rootJ, "mode", json_integer(mode));
            json_object_set_new(rootJ, "transpose_select", json_integer(transpose_select));

        return rootJ;
    }

    void dataFromJson(json_t *rootJ) override {
        json_t *scaleJ = json_object_get(rootJ, "scale");
        for (int i = 0; i < 12; i++) {
            json_t *semiJ = json_array_get(scaleJ, i);
            semiState[i] = !!json_integer_value(semiJ);
			semiLight[i] = semiState[i]?1.0:0.0;
        }
        json_t *modeJ = json_object_get(rootJ, "mode");
        if(modeJ) mode = (Mode) json_integer_value(modeJ);

        json_t *transJ = json_object_get(rootJ, "transpose_select");
        if(transJ) transpose_select = (bool) json_integer_value(transJ);

    }

	int modulo(int a, int b) {

		int r = a % b;
		return r < 0 ? r + b : r;

	}


};


void Quantum::process(const ProcessArgs &args) {

	for(int i=0; i<12; i++) {

		if (semiTriggers[i].process(params[Quantum::SEMI_1_PARAM + i].getValue())) {
                        semiState[i] = !semiState[i];
                }
		lights[i].value = semiState[i]?1.0f:0.0f;
	}

	float gate = 0.f, trigger=0.f;
	float quantized = 0.f;

	float v=inputs[IN_INPUT].getVoltage();
	float t=inputs[TRANSPOSE_INPUT].getNormalVoltage(0.0);

//	int octave   = floor(v);
//	int octave_t = floor(t);

	gate = mode==LAST? 0.0: 10.0;


	int semi_full   = round( 12.0f*v );
	int semi_t = round( 12.0f*t );

	int octave   = semi_full/12;

	int semi = semi_full % 12;

	if (semi<0) {
		semi+=12;
		octave-=1;
	}

	// transpose to shifted scale:

	int tmp_semi = modulo (semi-semi_t, 12) ;



   	if( inputs[RESET_INPUT].isConnected() ) {
		if( resetTrigger.process(inputs[RESET_INPUT].getVoltage()) ) onReset();
        };


	if( inputs[SET_INPUT].isConnected() ) {
		if( setTrigger.process(inputs[SET_INPUT].getVoltage() ) ) {

			float n=inputs[NOTE_INPUT].getNormalVoltage(0.0);
			int semi_n = round( 12.0f*(n - 1.0f*round(n)) ) - (transpose_select?semi_t:0);
			if(semi_n<0) semi_n+=12;

			semiState[semi_n] = !semiState[semi_n];
			semiLight[semi_n] = semiState[semi_n]?1.0:0.0;
		}
	};


	if( semiState[tmp_semi] )
	{
		bool changed = !( (octave==last_octave) && (semi==last_semi));
		gate = 10.0f;
		quantized = 1.0f*octave + semi/12.0f;
		if(changed) pulse.trigger(0.001);
		last_octave = octave;
		last_semi   = semi;

	} else {

		if(mode==LAST) {
			quantized = 1.0f*last_octave + last_semi/12.0f;
		} else {

			int i_up   = 0;
			int i_down = 0;

			while( !semiState[ modulo(tmp_semi+i_up,  12) ] && i_up   < 12 ) i_up++;
		 	while( !semiState[ modulo(tmp_semi-i_down,12) ] && i_down < 12 ) i_down++;
				
			switch(mode) {	
				case UP: 			if (i_up   < 12) semi += i_up;
									else {semi = last_semi; octave = last_octave; gate = 0.0f;}
									break;

				case DOWN: 			if (i_down < 12) semi -= i_down; 
									else {semi = last_semi; octave = last_octave; gate = 0.0f;}
									break;

				case CLOSEST_UP:   	if (i_up<12 && i_down<12) semi = (i_up > i_down) ? (semi - i_down) : (semi + i_up); 
									else {semi = last_semi; octave = last_octave; gate = 0.0f;}
									break;

				case CLOSEST_DOWN: 	if (i_up<12 && i_down<12) semi = (i_down > i_up) ? (semi + i_up) : (semi - i_down); 
									else {semi = last_semi; octave = last_octave; gate = 0.0f;}
									break;

				case LAST:	
				default:		break;
			};
				
			if( semi > 11 ) {
				semi   -= 12;
				octave += 1;
			}

			if( semi < 0 ) {
				semi   += 12;
				octave -= 1;
			}


			bool changed = !( (octave==last_octave)&&(semi==last_semi));
			quantized = 1.0f*octave + semi/12.0f;
			if(changed) pulse.trigger(0.001f);
			last_octave = octave;
			last_semi   = semi;
		};
	};

	float gSampleRate = args.sampleRate;

	trigger = pulse.process(1.0/gSampleRate) ? 10.0f : 0.0f;


	outputs[OUT_OUTPUT].setVoltage(quantized);
	outputs[GATE_OUTPUT].setVoltage(gate - trigger);
	outputs[TRIGGER_OUTPUT].setVoltage(trigger);

}

struct QuantumModeItem : MenuItem {

        Quantum *quantum;
        Quantum::Mode mode;

 
        void onAction(const event::Action &e) override {
                quantum->mode = mode;
        };

        void step() override {
                rightText = (quantum->mode == mode)? "✔" : "";
        };

};

struct QuantumTranposeModeItem : MenuItem {

	Quantum *quantum;
	bool transpose_select;

    void onAction(const event::Action &e) override {
            quantum->transpose_select = transpose_select;
  	};

    void step() override {
            rightText = (quantum->transpose_select == transpose_select)? "✔" : "";
    };

};

struct QuantumWidget : ModuleWidget {
	QuantumWidget(Quantum *module);
	json_t *dataToJsonData() ;
	void dataFromJsonData(json_t *root) ;
	void appendContextMenu(Menu *) override;
};

void QuantumWidget::appendContextMenu(Menu *menu) {



        Quantum *quantum = dynamic_cast<Quantum*>(module);
        assert(quantum);

        MenuLabel *modeLabel = new MenuLabel();
        modeLabel->text = "Quantizer Mode";
        menu->addChild(modeLabel);

        QuantumModeItem *last_Item = new QuantumModeItem();
        last_Item->text = "Last";
        last_Item->quantum = quantum;
        last_Item->mode = Quantum::LAST;
        menu->addChild(last_Item);

        QuantumModeItem *up_Item = new QuantumModeItem();
        up_Item->text = "Up";
        up_Item->quantum = quantum;
        up_Item->mode = Quantum::UP;
        menu->addChild(up_Item);

        QuantumModeItem *down_Item = new QuantumModeItem();
        down_Item->text = "Down";
        down_Item->quantum = quantum;
        down_Item->mode = Quantum::DOWN;
        menu->addChild(down_Item);

        QuantumModeItem *cl_up_Item = new QuantumModeItem();
        cl_up_Item->text = "Closest, up";
        cl_up_Item->quantum = quantum;
        cl_up_Item->mode = Quantum::CLOSEST_UP;
        menu->addChild(cl_up_Item);

        QuantumModeItem *cl_dn_Item = new QuantumModeItem();
        cl_dn_Item->text = "Closest, Down";
        cl_dn_Item->quantum = quantum;
        cl_dn_Item->mode = Quantum::CLOSEST_DOWN;
        menu->addChild(cl_dn_Item);

		MenuLabel *selectLabel = new MenuLabel();
        selectLabel->text = "Note Select";
        menu->addChild(selectLabel);

		QuantumTranposeModeItem *trans_select_on_Item = new QuantumTranposeModeItem();
		trans_select_on_Item->text = "Transpose";
		trans_select_on_Item->quantum = quantum;
		trans_select_on_Item->transpose_select = true;
		menu->addChild(trans_select_on_Item);

		QuantumTranposeModeItem *trans_select_off_Item = new QuantumTranposeModeItem();
		trans_select_off_Item->text = "Don't Transpose";
		trans_select_off_Item->quantum = quantum;
		trans_select_off_Item->transpose_select = false;
		menu->addChild(trans_select_off_Item);


};

QuantumWidget::QuantumWidget(Quantum *module) {
		
	setModule(module);
	box.size = Vec(15*8, 380);

	{
		SvgPanel *panel = new SvgPanel();
		panel->box.size = box.size;
		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance,"res/Quantum.svg")));
		addChild(panel);
	}

	addChild(createWidget<MLScrew>(Vec(15, 0)));
	addChild(createWidget<MLScrew>(Vec(box.size.x-30, 0)));
	addChild(createWidget<MLScrew>(Vec(15, 365)));
	addChild(createWidget<MLScrew>(Vec(box.size.x-30, 365)));

	addInput(createInput<MLPort>(Vec(19, 42), module, Quantum::IN_INPUT));
	addOutput(createOutput<MLPort>(Vec(75, 42), module, Quantum::OUT_OUTPUT));

	addInput(createInput<MLPort>(Vec(75, 90), module, Quantum::TRANSPOSE_INPUT));
	addOutput(createOutput<MLPort>(Vec(75, 140), module, Quantum::GATE_OUTPUT));
	addOutput(createOutput<MLPort>(Vec(75, 180), module, Quantum::TRIGGER_OUTPUT));

	addInput(createInput<MLPort>(Vec(75, 226), module, Quantum::NOTE_INPUT));
	addInput(createInput<MLPort>(Vec(75, 266), module, Quantum::SET_INPUT));
	addInput(createInput<MLPort>(Vec(75, 312), module, Quantum::RESET_INPUT));

	static const float offset_x = 24;
	static const float offset_y = 333;

	for(int i=0; i<12; i++) {
		addParam(createParam<ML_SmallLEDButton>(Vec(offset_x, -22*i+offset_y), module, Quantum::SEMI_1_PARAM + i));
		addChild(createLight<MLSmallLight<GreenLight>>(Vec(offset_x+4, -22*i+4+offset_y), module, Quantum::SEMI_1_LIGHT+i));
	}

}

Model *modelQuantum = createModel<Quantum, QuantumWidget>("Quantum");
