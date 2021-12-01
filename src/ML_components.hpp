#include "rack.hpp"

#include <sstream>
#include <iomanip>      // std::setw

#pragma once

using namespace rack;


extern Plugin *pluginInstance;
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

struct MLKnob : app::SvgKnob {
	widget::SvgWidget* bg;

	MLKnob() {
		minAngle = -0.83 * M_PI;
		maxAngle = 0.83 * M_PI;

		bg = new widget::SvgWidget;
		fb->addChildBelow(bg, tw);
	}
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


struct MLPort : SvgPort {
	MLPort();
};

struct MLPortOut : SvgPort {
	MLPortOut();
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


template<typename T> struct NumberDisplayWidget : TransparentWidget {

  T *value = nullptr;
  bool *on = nullptr;

  int num_digits, num_decimals;
  int extra;
  float margin;
  std::string fontPath;
  const float char_width =13.f; // to be replaced by code to extract the real size of a character
  const float char_height=18.f; // to be replaced by code to extract the real size of a character
  const float point_width = 5.f;
  const float spacing = 2.5f;

  NumberDisplayWidget(int ndigits, int ndecimals=0, float m=5.f);
  void draw(const DrawArgs &args) override;
  void drawLayer(const DrawArgs &args, int layer) override;
  float getWidth();

};

template <typename T>
NumberDisplayWidget<T>::NumberDisplayWidget(int ndigits, int ndecimals, float my_margin) {

  num_digits = ndigits;
  num_decimals = ndecimals;
  margin = my_margin;

  int total = char_width*num_digits;
  if(num_decimals>0) {total+=char_width*num_decimals+point_width;} 
	this->fontPath = "res/Segment7Standard.ttf";
	this->box.size = Vec(2*margin + total - spacing, 20);
};

template <typename T>
void NumberDisplayWidget<T>::draw(const DrawArgs &args)  {

    NVGcolor backgroundColor = nvgRGB(0x20, 0x20, 0x20);
    NVGcolor borderColor = nvgRGB(0x10, 0x10, 0x10);

    nvgBeginPath(args.vg);
    nvgRoundedRect(args.vg, 0.0, 0.0, box.size.x, box.size.y, 4.0);
    nvgFillColor(args.vg, backgroundColor);
    nvgFill(args.vg);

    nvgStrokeWidth(args.vg, 1.0);
    nvgStrokeColor(args.vg, borderColor);
    nvgStroke(args.vg);

};


template <typename T>
void NumberDisplayWidget<T>::drawLayer(const DrawArgs &args, int layer)  {
  // Background
  NVGcolor backgroundColor = nvgRGB(0x10, 0x10, 0x10);
  NVGcolor textColor;

  std::shared_ptr<Font> font = APP->window->loadFont(asset::plugin(pluginInstance, this->fontPath));

  Vec textPos = Vec(margin, 17.0f);
  
  std::stringstream to_display;
	std::string bg = "";
  int extra=0;

  nvgBeginPath(args.vg);
  nvgRoundedRect(args.vg, 0.0, 0.0, box.size.x, box.size.y, 4.0);
  nvgFillColor(args.vg, backgroundColor);
  nvgFill(args.vg);

  nvgFontSize(args.vg, 18);
  nvgFontFaceId(args.vg, font->handle);
  nvgTextLetterSpacing(args.vg, 2.5);

  for(int i=0; i<num_digits; i++) {
    bg += "8";
  };
  if(num_decimals>0) { bg+='.'; extra=1; }
  for(int i=0; i<num_decimals; i++) {
    bg += "8";
  };

//	if(value) {to_display = std::to_string(*value);}

  if(value) {
    to_display << std::setw(extra+num_digits+num_decimals) << std::setprecision(num_decimals) << std::fixed << *value;
  } 
  else {
    to_display << " ";
  }

//  while(to_display.length()<num_digits) {
//		to_display = ' ' + to_display;
//  };


  textColor = nvgRGB(0xdf, 0xd2, 0x2c);
  nvgFillColor(args.vg, nvgTransRGBA(textColor, 16));
  nvgText(args.vg, textPos.x, textPos.y, bg.c_str(), NULL);

  if(!on || *on ) {
    textColor = nvgRGB(0xf0, 0x00, 0x00);
    nvgFillColor(args.vg, textColor);
    nvgText(args.vg, textPos.x, textPos.y, to_display.str().c_str(), NULL);
  }
};



struct SmallNumberDisplayWidget : TransparentWidget {

  int *value = 0;
  std::shared_ptr<Font> font;

  SmallNumberDisplayWidget();
  void drawLayer(const DrawArgs &args, int layer) override;

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

struct NKK2 : SvgSwitch { //Toggle
	NKK2() {
		addFrame(APP->window->loadSvg(asset::system("res/ComponentLibrary/NKK_0.svg")));
		addFrame(APP->window->loadSvg(asset::system("res/ComponentLibrary/NKK_2.svg")));
	}
};

struct POLSWITCH : SvgSwitch { //Toggle
	POLSWITCH() {
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CKSS_0.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CKSS_1.svg")));
	}
};
