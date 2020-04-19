#include "ML_modules.hpp"

#include "dsp/digital.hpp"
#include "osdialog.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <list>

using namespace rack;
using namespace dsp;

#define MAX(a,b) (a>b)?a:b
#define MIN(a,b) (a<b)?a:b

#define PORT_MAX_CHANNELS 16

#define DISPLAY_WIDTH  60
#define DISPLAY_HEIGHT 120



enum NoteOrder {
	PITCH,
	TIME
};

enum Mode {
	UP,
	DOWN,
	UPDOWN1,
	DOWNUP1,
	UPDOWN2,
	DOWNUP2,
	RANDOM,
	NUM_MODES
};


struct NodeData {

	NodeData() {
		valid = false;
		pitch = 0.0f;
		cv1 = 0.0f;
		cv2 = 0.0f;
	}

	void reset() {
		valid = false;
		pitch = 0.0f;
		cv1 = 0.0f;
		cv2 = 0.0f;
	}

	void set(float p, float v1=0.f, float v2=0.f) {
		valid = true;
		pitch = p;
		cv1 = v1;
		cv2 = v2;
	}

	NodeData(float p, float v1=0.f, float v2=0.f) {
		valid = true;
		pitch = p;
		cv1 = v1;
		cv2 = v2;
	}

	bool valid;
	float pitch;
	float cv1;
	float cv2;

};


struct Driver {

/* Examples:

Range = 2, number_of_notes = 3
UP:

0 1 2 0 1 2 0 1 2 0 1 2 ...
0 0 0 1 1 1 0 0 0 1 1 1 ...

DOWN:

2 1 0 2 1 0 2 1 0 2 1 0 ...
1 1 1 0 0 0 1 1 1 0 0 0 ...

UPDOWN1:

0 1 2 0 1  2 1 0 2 1  0 1 2 0 1  2 ...
0 0 0 1 1  1 1 1 0 0  0 0 0 1 1  1 ...

UPDOWN2:

0 1 2 0 1 2  2 1 0 2 1 0  0 1 2 0 1 2 ...
0 0 0 1 1 1  1 1 1 0 0 0  0 0 0 1 1 1 ...
*/

public:

	Driver() {mode = Mode::UP; current = 0; octave = 0; range = 0; step = 1;};

	inline int getIndex(int num_of_notes) {return current<0?0:(current<num_of_notes?current:num_of_notes-1) ;}
	inline int getOctave() {return octave;}

	inline void reset() {
		switch(mode) {
			case UP:		
			case UPDOWN1: 
			case UPDOWN2:   current = -1; octave = 0;     step =  1; break;
			case DOWN:		
			case DOWNUP1:
			case DOWNUP2:   current = -2; octave = range; step = -1; break;
			default: current = 0; octave = 0; step = 0;
		}
	}

	inline int rand_range(int min, int max) {float rnd=rand()/(RAND_MAX-1.0); return min + roundf(rnd*(max-min)); }


	inline void setMode(Mode m) {

		if(mode!=m) {
			switch(m) {
				case UP:     step =  1; break;
				case DOWN:   step = -1; break;
				case RANDOM: step =  0; break;
				case UPDOWN1:
				case UPDOWN2: if(step==0) step = 1; break;
				case DOWNUP1:
				case DOWNUP2: if(step==0) step = -1; break;
				default: ;	
			};
			mode = m;
			// reset();
		}
	}


	inline void setRange(int r) {range=r;}

	void process(int);


private:

	Mode mode;
	int range;
	int step;

	int current;
	int octave;

};


void Driver::process(int number_of_notes) {

	// possible states at beginning of process():
	//
	// counter = -2: init state for DOWN modes
	// counter = -1: init state for UP modes
	// counter = 0-N (where N is number_of_notes-1 during last call and might be above current number_of_notes)
	//
	// octave = 0-N  (where N is range during last call)
	//
	// step = -1, 0, 1
	
	if(current < -1) current = number_of_notes;

	current += step;

	switch(mode) {

		case UP:	if(current >= number_of_notes) {
						current = 0; octave += step;
					}
					if(current<0) current=0;
					if(octave > range) octave = 0;
					break;

		case DOWN:	if(current < 0) {
						current = number_of_notes-1; octave += step;
					};
					if(current>number_of_notes-1) current=number_of_notes-1;
					if(octave<0) octave = range;
					break;

		case UPDOWN1:
		case DOWNUP1:
					if(octave >= range && current >= number_of_notes-1 ) {
						step = -1;
					}
					if(current >= number_of_notes) {
						current = 0; 
						if(octave<range) octave += step; // in this case step= 1
					}

					if(octave <= 0 && current < 1 ) {
						step = 1;
					}
					if(current < 0) {
						current = number_of_notes-1; 
						if(octave>=0) octave += step;   // in this case step=-1
					};
					break;
		case UPDOWN2:
		case DOWNUP2:
					if(current >= number_of_notes && octave > range-1) {
						current -= step;
						step = -1;
					}
					if(current >= number_of_notes) {
						current = 0; 
						if(octave<range) octave += step;
					}

					if(current < 0 && octave < 1) {
						current += 1;
						step = 1;
					}
					if(current < 0) {
						current = number_of_notes-1; 
						if(octave>0) octave += step;
					};
					break;
		case RANDOM: 
					current = rand_range(0, number_of_notes-1);
					octave =  rand_range(0, range);
					break;
		default: ;
	}

	if(octave>range) octave = range;

}

struct Arpeggiator : Module {

	enum ParamIds {
		ORDER_PARAM,
		CV1_MODE_PARAM,
		CV2_MODE_PARAM,
		MODE_UP_PARAM,
		MODE_DN_PARAM,
		RANGE_UP_PARAM,
		RANGE_DN_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		GATE_INPUT, PITCH_INPUT, CV1_INPUT, CV2_INPUT, CV3_INPUT, // keyboard input
		TRIG_INPUT,           // clock input
		MODE_INPUT, RANGE_INPUT, ORDER_INPUT, RESET_INPUT, HOLD_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		GATE_OUTPUT, PITCH_OUTPUT, CV1_OUTPUT, CV2_OUTPUT, CV3_OUTPUT, // keyboard input
		NUM_NOTES_OUTPUT,
		NUM_OUTPUTS 
	};
	enum LightIds {
		NUM_LIGHTS
	};

	Arpeggiator() {
		config( NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS );

		configParam(ORDER_PARAM, 0.0, 1.0, 0.0, "Note Order");


		configParam(CV1_MODE_PARAM, -1.0, 1.0, 0.0, "CV1 Mode"); // -1 = S&H in, 0 = continuous, 1 = S&H out
		configParam(CV2_MODE_PARAM, -1.0, 1.0, 0.0, "CV2 Mode"); // -1 = S&H in, 0 = continuous, 1 = S&H out

		order_global = 0;
		mode_global = 0;
		range_global = 0;

		onReset();


	};

	int channels_trigger;
	int mode_display[PORT_MAX_CHANNELS];
	int range_display[PORT_MAX_CHANNELS];
	int order_display[PORT_MAX_CHANNELS];

	void step() override;

	void onAdd() override {

	}

	void onReset() override {
		noteOrder = NoteOrder::TIME;
		mode = Mode::UP;	
		for(int c=0; c<PORT_MAX_CHANNELS; c++) {
			driver[c].reset();
			driver[c].setMode(mode);
			driver[c].setRange(1);
			gate_mask[c] = 0.0f;
			channelList[c].reset();
			index[c] = 0;
			pitch_cv[c] = 0.0f;
			lookup[NoteOrder::PITCH][c] = 0;
			lookup[NoteOrder::TIME ][c] = 0;

			order_display[c] = 0;
			mode_display[c] = 0;
			range_display[c] = 1;

			cv1_sh_o[c] = 0.0f;
			cv2_sh_o[c] = 0.0f;

			upTrigger[c].process(0.0f);
			dnTrigger[c].process(0.0f);

		}
		pitchOrderList.clear();
		playOrderList.clear();
		number_of_notes = 0;
		channels_trigger = 1;

		channels_last = 0;
		hold_last = false;

	};

	json_t *dataToJson() override {

		json_t *rootJ = json_object();

		json_object_set_new(rootJ, "order", json_integer(order_global));
		json_object_set_new(rootJ, "range", json_integer(range_global));
		json_object_set_new(rootJ, "mode",  json_integer(mode_global));
		
		return rootJ;

	};

	void dataFromJson(json_t *rootJ) override {

		json_t *orderJ = json_object_get(rootJ, "order");
		if(orderJ) order_global = json_integer_value(orderJ);

		json_t *rangeJ = json_object_get(rootJ, "range");
		if(rangeJ) range_global = json_integer_value(rangeJ);

		json_t *modeJ = json_object_get(rootJ, "mode");
		if(modeJ) mode_global = json_integer_value(modeJ);

	};

	private: 

	dsp::SchmittTrigger upTrigger[PORT_MAX_CHANNELS];
	dsp::SchmittTrigger dnTrigger[PORT_MAX_CHANNELS];

	dsp::SchmittTrigger noteTrigger[PORT_MAX_CHANNELS];
	dsp::SchmittTrigger resetTrigger[PORT_MAX_CHANNELS];

	dsp::SchmittTrigger orderTrigger;
	dsp::SchmittTrigger rangeUpTrigger;
	dsp::SchmittTrigger rangeDnTrigger;
	dsp::SchmittTrigger modeUpTrigger;
	dsp::SchmittTrigger modeDnTrigger;

	std::list<int> playOrderList;
	std::list<int> pitchOrderList;

	int number_of_notes;
	int channels_last;

	bool hold_last;

	int order_global;
	int mode_global;
	int range_global;



	Driver driver[PORT_MAX_CHANNELS];

	NodeData channelList[PORT_MAX_CHANNELS];

	int lookup[2][PORT_MAX_CHANNELS];
	int index[PORT_MAX_CHANNELS];

	NoteOrder noteOrder;
	Mode mode;

	float pitch_cv[PORT_MAX_CHANNELS];
	float gate_mask[PORT_MAX_CHANNELS];

	float cv1_sh_o[PORT_MAX_CHANNELS];
	float cv2_sh_o[PORT_MAX_CHANNELS];

	void calculateLookup();


};


void Arpeggiator::calculateLookup() {

	std::list<int>::iterator itPitch=pitchOrderList.begin();
	std::list<int>::iterator itPlay =playOrderList.begin();
			
	for(int i=0; i<number_of_notes; i++) {
		lookup[NoteOrder::PITCH][i] = *itPitch++;
		lookup[NoteOrder::TIME][i]  = *itPlay++;
	}

	for(int i=number_of_notes; i<PORT_MAX_CHANNELS; i++) {
		lookup[NoteOrder::PITCH][i] = 0;
		lookup[NoteOrder::TIME][i]  = 0;
	}

}


void Arpeggiator::step() {

	// handle note list
	// ----------------

	int channels_pitch   = inputs[PITCH_INPUT].getChannels();
	int channels_gate    = inputs[GATE_OUTPUT].getChannels();

	int channels = MIN(channels_pitch, channels_gate);

	// if hold is active:
	// - notes will not be removed if their gate goes down.
	// - triggers on a channel, holding a note, will replace that note
	// if hold goes down:
	// - held notes notes with low gate will be released. 

	bool hold = inputs[HOLD_INPUT].getNormalVoltage(0.0f) > 0.5;

	if(channels != channels_last) onReset();

	channels_last = channels;

	float cv1_s_h = params[CV1_MODE_PARAM].getValue();
	float cv2_s_h = params[CV2_MODE_PARAM].getValue();

	for(int c=0; c<channels; c++) {

		float gate  = inputs[GATE_INPUT].getNormalPolyVoltage(0.0f, c);

		if( upTrigger[c].process(gate) ) {

			float pitch = inputs[PITCH_INPUT].getNormalPolyVoltage(0.0f, c);
			float cv1   = inputs[CV1_INPUT].getNormalPolyVoltage(0.0f, c);
			float cv2   = inputs[CV2_INPUT].getNormalPolyVoltage(0.0f, c);


			if(!channelList[c].valid) {
				channelList[c].set(pitch, cv1, cv2);
				playOrderList.push_back(c);

				std::list<int>::iterator it = pitchOrderList.begin(); 
				while(it != pitchOrderList.end() && channelList[*it].pitch < pitch) it++;
				pitchOrderList.insert(it, c);

				number_of_notes++;

				calculateLookup();
			}

		}

		if(!hold && dnTrigger[c].process(5.f-gate) ) {

			if(number_of_notes>0) {
				if(channelList[c].valid) {
					playOrderList.remove(c);
					pitchOrderList.remove(c);
					channelList[c].reset();
					number_of_notes--;
					calculateLookup();
				}
			}
		}; 

		if(!hold && hold!=hold_last) {
			if(channelList[c].valid && !upTrigger[c].isHigh() ) {
					playOrderList.remove(c);
					pitchOrderList.remove(c);
					channelList[c].reset();
					number_of_notes--;
					calculateLookup();
			}
		}

	}

	hold_last = hold;

	// handle arpeggiator
	// ------------------

	channels_trigger = inputs[TRIG_INPUT].getChannels();

	outputs[PITCH_OUTPUT].setChannels(channels_trigger);
	outputs[GATE_OUTPUT ].setChannels(channels_trigger);
	outputs[CV1_OUTPUT  ].setChannels(channels_trigger);
	outputs[CV2_OUTPUT  ].setChannels(channels_trigger);


	if( orderTrigger.process(params[ORDER_PARAM].getValue()) ) order_global = 1 - order_global;

	if( rangeUpTrigger.process(params[RANGE_UP_PARAM].getValue()) ) range_global = range_global==3?0:range_global+1;
	if( rangeDnTrigger.process(params[RANGE_DN_PARAM].getValue()) ) range_global = range_global==0?3:range_global-1;

	if( modeUpTrigger.process(params[MODE_UP_PARAM].getValue()) ) mode_global = mode_global==NUM_MODES-1?0:mode_global+1;
	if( modeDnTrigger.process(params[MODE_DN_PARAM].getValue()) ) mode_global = mode_global==0?NUM_MODES-1:mode_global-1;

	for(int c=0; c<channels_trigger; c++) {

		order_display[c] = (int) clamp(order_global + inputs[ORDER_INPUT].getNormalPolyVoltage(0.0f, c), 0.0f, 1.0f);
		noteOrder = (NoteOrder) order_display[c];

		float gate = inputs[TRIG_INPUT].getNormalPolyVoltage(0.f, c);
		int channel_range = round(inputs[RANGE_INPUT].getNormalPolyVoltage(0.f, c));
		int channel_mode  = round(inputs[MODE_INPUT ].getNormalPolyVoltage(0.f, c));

		mode_display[c]  = clamp(mode_global  + channel_mode,  0, NUM_MODES-1);
		range_display[c] = clamp(range_global + channel_range, 0, 3);


		if(resetTrigger[c].process(inputs[RESET_INPUT].getNormalPolyVoltage(0.0f, c))) driver[c].reset();
		
		if( noteTrigger[c].process(gate) ) {

			gate_mask[c] = number_of_notes>0?1.0:0.0;
		

			driver[c].setMode( (Mode) mode_display[c] );
			driver[c].setRange(range_display[c]);
			driver[c].process(number_of_notes);

			index[c] = lookup[noteOrder][driver[c].getIndex(number_of_notes)];
	
			float octave = driver[c].getOctave();
			// pitch_cv[c] = octave + (channelList[index[c]].valid?channelList[index[c]].pitch:-10.0f);
			pitch_cv[c] = octave + (channelList[index[c]].valid?channelList[index[c]].pitch:pitch_cv[c]);

			cv1_sh_o[c] = inputs[CV1_INPUT].getNormalPolyVoltage(0.0f, index[c]);
			cv2_sh_o[c] = inputs[CV2_INPUT].getNormalPolyVoltage(0.0f, index[c]);

		}
				
		int index_local = index[c];

		float cv1 = cv1_s_h < -0.5f? channelList[index_local].cv1 : ( cv1_s_h > 0.5f ? cv1_sh_o[c] : inputs[CV1_INPUT].getNormalPolyVoltage(0.0f, index_local));
		float cv2 = cv2_s_h < -0.5f? channelList[index_local].cv2 : ( cv2_s_h > 0.5f ? cv2_sh_o[c] : inputs[CV2_INPUT].getNormalPolyVoltage(0.0f, index_local));


		outputs[GATE_OUTPUT].setVoltage(gate_mask[c]*gate, c);
		outputs[PITCH_OUTPUT].setVoltage(pitch_cv[c], c);
		outputs[CV1_OUTPUT].setVoltage(cv1, c);
		outputs[CV2_OUTPUT].setVoltage(cv2, c);

	} // c loop
	
};


struct ArpDisplayWidget : TransparentWidget {

	Arpeggiator* arp;
	float height;
	float width;

	ArpDisplayWidget() {
		box.size = Vec(DISPLAY_WIDTH, DISPLAY_HEIGHT);
		height = DISPLAY_HEIGHT/17.f;
	}

	void draw(const DrawArgs &args) override {

		const auto vg=args.vg;

		nvgSave(vg);

		if(arp) {

			int channels = MAX(arp->channels_trigger,1);
			width = (DISPLAY_WIDTH) / (1.0f * channels);

			nvgBeginPath(vg);
			nvgStrokeWidth(vg, 0.5);

			nvgMoveTo(vg, 0, 0);
			nvgLineTo(vg, 0, DISPLAY_HEIGHT);
			nvgLineTo(vg, DISPLAY_WIDTH, DISPLAY_HEIGHT);
			nvgLineTo(vg, DISPLAY_WIDTH, 0);
			nvgLineTo(vg, 0, 0);
			nvgClosePath(vg);
			nvgStrokeColor(vg, nvgRGBA(20,20,20,255));
			nvgStroke(vg);

			nvgBeginPath(vg);
			nvgStrokeWidth(vg, 0.5);

			nvgMoveTo(vg, 0, 1*height);
			nvgLineTo(vg, DISPLAY_WIDTH, 1*height);
			nvgMoveTo(vg, 0, 2*height);
			nvgLineTo(vg, DISPLAY_WIDTH, 2*height);

			nvgMoveTo(vg, 0, 4*height);
			nvgLineTo(vg, DISPLAY_WIDTH, 4*height);
			nvgMoveTo(vg, 0, 5*height);
			nvgLineTo(vg, DISPLAY_WIDTH, 5*height);
			nvgMoveTo(vg, 0, 6*height);
			nvgLineTo(vg, DISPLAY_WIDTH, 6*height);
			nvgMoveTo(vg, 0, 7*height);
			nvgLineTo(vg, DISPLAY_WIDTH, 7*height);
			nvgMoveTo(vg, 0, 8*height);
			nvgLineTo(vg, DISPLAY_WIDTH, 8*height);

			nvgMoveTo(vg, 0, 10*height);
			nvgLineTo(vg, DISPLAY_WIDTH, 10*height);
			nvgMoveTo(vg, 0, 11*height);
			nvgLineTo(vg, DISPLAY_WIDTH, 11*height);
			nvgMoveTo(vg, 0, 12*height);
			nvgLineTo(vg, DISPLAY_WIDTH, 12*height);
			nvgMoveTo(vg, 0, 13*height);
			nvgLineTo(vg, DISPLAY_WIDTH, 13*height);
			nvgMoveTo(vg, 0, 14*height);
			nvgLineTo(vg, DISPLAY_WIDTH, 14*height);
			nvgMoveTo(vg, 0, 15*height);
			nvgLineTo(vg, DISPLAY_WIDTH, 15*height);
			nvgMoveTo(vg, 0, 16*height);
			nvgLineTo(vg, DISPLAY_WIDTH, 16*height);

			nvgClosePath(vg);
			nvgStrokeColor(vg, nvgRGBA(20,20,20,255));
			nvgStroke(vg);


			for(int c=0; c<channels; c++) {

				nvgBeginPath(vg);
				nvgMoveTo(vg, c*width, 0);
				nvgLineTo(vg, c*width, DISPLAY_HEIGHT);
				nvgClosePath(vg);
				nvgStrokeColor(vg, nvgRGBA(20,20,20,255));
				nvgStroke(vg);
			
				nvgBeginPath(vg);
				nvgRect(vg, c*width, height*(1-arp->order_display[c]), width, height);
				nvgFillColor(vg, nvgRGBA(20,20,20,200));
				nvgFill(vg);

				nvgBeginPath(vg);
				nvgRect(vg, c*width, height*(4+ 3-arp->range_display[c]), width, height);
				nvgFillColor(vg, nvgRGBA(20,20,20,200));
				nvgFill(vg);

				nvgBeginPath(vg);
				nvgRect(vg, c*width, height*(9+ 7-arp->mode_display[c]), width, height);
				nvgFillColor(vg, nvgRGBA(20,20,20,200));
				nvgFill(vg);

			}

		}

		nvgRestore(vg);
	};
};

struct ArpeggiatorWidget:ModuleWidget {

	ArpeggiatorWidget(Arpeggiator* module);	
	Arpeggiator* arp;

};

ArpeggiatorWidget::ArpeggiatorWidget(Arpeggiator* module) {

	setModule(module);
	arp = module;

	box.size = Vec(15*10, 380);
	
	{
		SvgPanel *panel = new SvgPanel();
		panel->box.size = box.size;
		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance,"res/Arpeggiator.svg")));
		addChild(panel);
	}
	
	if(arp) {
		ArpDisplayWidget* display = new ArpDisplayWidget();
		display->box.pos = Vec(31,35);
		display->arp = arp;
		addChild(display);
	}
	
	addChild(createWidget<MLScrew>(Vec(15, 0)));
	addChild(createWidget<MLScrew>(Vec(box.size.x-30, 0)));
	addChild(createWidget<MLScrew>(Vec(15, 365)));
	addChild(createWidget<MLScrew>(Vec(box.size.x-30, 365)));

	addParam(createParam<MLSmallButton>( Vec( 7, 35), module, Arpeggiator::ORDER_PARAM));
	addParam(createParam<MLSmallButtonUp>( Vec( 7, 60), module, Arpeggiator::RANGE_UP_PARAM));
	addParam(createParam<MLSmallButtonDn>( Vec( 7, 80), module, Arpeggiator::RANGE_DN_PARAM));
	addParam(createParam<MLSmallButtonUp>( Vec( 7, 110), module, Arpeggiator::MODE_UP_PARAM));
	addParam(createParam<MLSmallButtonDn>( Vec( 7, 130), module, Arpeggiator::MODE_DN_PARAM));


	addInput(createInput<MLPort>(   Vec(115, 35),   module, Arpeggiator::ORDER_INPUT));
	addInput(createInput<MLPort>(   Vec(115, 75),   module, Arpeggiator::RANGE_INPUT));
	addInput(createInput<MLPort>(   Vec(115, 115),  module, Arpeggiator::MODE_INPUT));


	addInput(createInput<MLPort>( Vec(75, 225),      module, Arpeggiator::PITCH_INPUT));
	addInput(createInput<MLPort>( Vec(75, 260),      module, Arpeggiator::GATE_INPUT));
	addInput(createInput<MLPort>( Vec(75, 295),      module, Arpeggiator::CV1_INPUT));
	addInput(createInput<MLPort>( Vec(75, 330),      module, Arpeggiator::CV2_INPUT));

    addParam(createParam<MLSliderHoriz3>(   Vec(35, 300),       module, Arpeggiator::CV1_MODE_PARAM));
    addParam(createParam<MLSliderHoriz3>(   Vec(35, 335),       module, Arpeggiator::CV2_MODE_PARAM));

	addInput(createInput<MLPort>( Vec(25, 175),       module, Arpeggiator::HOLD_INPUT));
	addInput(createInput<MLPort>( Vec(75, 175),       module, Arpeggiator::RESET_INPUT));
	addInput(createInput<MLPort>( Vec(115, 175),      module, Arpeggiator::TRIG_INPUT));

	addOutput(createOutput<MLPort>( Vec(115, 225),    module, Arpeggiator::PITCH_OUTPUT));
	addOutput(createOutput<MLPort>( Vec(115, 260),    module, Arpeggiator::GATE_OUTPUT));
	addOutput(createOutput<MLPort>( Vec(115, 295),    module, Arpeggiator::CV1_OUTPUT));
	addOutput(createOutput<MLPort>( Vec(115, 330),    module, Arpeggiator::CV2_OUTPUT));

};



Model *modelArpeggiator = createModel<Arpeggiator, ArpeggiatorWidget>("Arpeggiator");
