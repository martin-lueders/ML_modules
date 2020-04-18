#include "rack.hpp"

#pragma once

using namespace rack;

#define MAX(a,b) a>b?a:b
#define MIN(a,b) a>b?b:a


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

struct MLWhiteLight : ModuleLightWidget {
	MLWhiteLight();
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

	MLScrew();
};


struct NumberDisplayWidget : TransparentWidget {

  int *value = 0;
  std::shared_ptr<Font> font;

  NumberDisplayWidget();
  void draw(const DrawArgs &args) override;

};

struct SmallNumberDisplayWidget : TransparentWidget {

  int *value = 0;
  std::shared_ptr<Font> font;

  SmallNumberDisplayWidget();
  void draw(const DrawArgs &args) override;

};

struct MLSliderHoriz3 : SvgSwitch {

	MLSliderHoriz3();

};

struct MLSmallButtonUp : SvgSwitch {
	MLSmallButtonUp();
};

struct MLSmallButtonDn : SvgSwitch {
	MLSmallButtonDn();
};
