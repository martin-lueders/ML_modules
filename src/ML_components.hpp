#include "rack.hpp"

#pragma once

using namespace rack;

extern Plugin *pluginInstance;

/*
struct MLSVGSwitch : SvgSwitch {

	// CircularShadow *shadow;

	// std::vector<std::shared_ptr<Svg>> frames;
	// SvgWidget *sw;

	MLSVGSwitch();
	// Adds an SVG file to represent the next switch position 
	// void addFrame(std::shared_ptr<Svg> svg);
};
*/


template <typename BASE>
struct MLLargeLight : BASE {
	MLLargeLight() {
		this->box.size = Vec(16.0, 16.0);
	}
};

template <typename BASE>
struct MLMediumLight : BASE {
	MLMediumLight() {
		this->box.size = Vec(12.0, 12.0);
	}
};


template <typename BASE>
struct MLSmallLight : BASE {
	MLSmallLight() {
		this->box.size = Vec(8.0, 8.0);
	}
};

struct WhiteLight : ModuleLightWidget {
	WhiteLight();
};


struct BlueMLKnob : RoundKnob {
        BlueMLKnob();
};

struct SmallBlueMLKnob : RoundKnob {
        SmallBlueMLKnob();
};

struct BlueSnapMLKnob : RoundKnob {
        BlueSnapMLKnob();
};

struct SmallBlueSnapMLKnob : RoundKnob {
        SmallBlueSnapMLKnob();
};


struct RedMLKnob : RoundKnob {
        RedMLKnob();
};

struct SmallRedMLKnob : RoundKnob {
        SmallRedMLKnob();
};

struct RedSnapMLKnob : RoundKnob {
        RedSnapMLKnob();
};

struct SmallRedSnapMLKnob : RoundKnob {
        SmallRedSnapMLKnob();
};



struct GreyMLKnob : RoundKnob {
        GreyMLKnob();
};

struct SmallGreyMLKnob : RoundKnob {
        SmallGreyMLKnob();
};


struct GreySnapMLKnob : RoundKnob {
        GreySnapMLKnob();
};

struct SmallGreySnapMLKnob : RoundKnob {
        SmallGreySnapMLKnob();
};


struct MLPort : SvgPort {
	MLPort();
};


struct MLButton : SvgSwitch { //Momentary
	MLButton();
};

struct MLSmallButton : SvgSwitch { // Momentary
	MLSmallButton();
};

struct ML_ResetButton : SvgSwitch { //Momentary
	ML_ResetButton();
};

struct ML_LEDButton : SvgSwitch  { //Momentary
	
	ML_LEDButton();
};

struct ML_MediumLEDButton : SvgSwitch { //Momentary
	
	ML_MediumLEDButton();
};


struct ML_SmallLEDButton : SvgSwitch { //Momentary
	
	ML_SmallLEDButton();
};



struct MLSwitch : SvgSwitch { //Toggle

	MLSwitch();
};

struct MLSwitch2 : SvgSwitch { //Toggle
	MLSwitch2();
};

struct BlueMLSwitch : SvgSwitch { //Toggle
	BlueMLSwitch();
};



struct MLScrew : FramebufferWidget {

    SvgWidget *sw;
    TransformWidget *tw;

	MLScrew() {

        tw = new TransformWidget();
	    addChild(tw);
	    sw = new SvgWidget();
	    tw->addChild(sw);
	    sw->setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/MLScrew.svg")));
		tw->box.size = sw->box.size;	

        float angle = 1.71f * (rand() / (static_cast<double>(RAND_MAX) + 1.0)); 

        Vec transl = tw->box.getCenter();
        tw->translate( transl );
        tw->rotate(angle);
        tw->translate( transl.neg() );

	}

    
};
