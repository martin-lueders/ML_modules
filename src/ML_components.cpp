#include "ML_components.hpp"


#include <functional>

using namespace rack;

extern Plugin *pluginInstance;



MLSVGSwitch::MLSVGSwitch() {

    shadow = new CircularShadow();
    addChild(shadow);
    shadow->box.size = Vec();

	sw = new SVGWidget();
	addChild(sw);
}

void MLSVGSwitch::addFrame(std::shared_ptr<SVG> svg) {
	frames.push_back(svg);
	// If this is our first frame, automatically set SVG and size
	if (!sw->svg) {
		sw->setSVG(svg);
		box.size = sw->box.size;
	}
}

void MLSVGSwitch::onChange(event::Change &e) {
	assert(frames.size() > 0);
	float valueScaled = rescale(value, minValue, maxValue, 0, frames.size() - 1);
	int index = clamp((int) roundf(valueScaled), 0, (int) frames.size() - 1);
	sw->setSVG(frames[index]);
	dirty = true;
	ParamWidget::onChange(e);
}




WhiteLight::WhiteLight() {
	addBaseColor(COLOR_WHITE);
}

BlueMLKnob::BlueMLKnob() {
    setSVG(APP->window->loadSvg(asset::plugin(pluginInstance,"res/Knob.svg")));
};

SmallBlueMLKnob::SmallBlueMLKnob() {
    setSVG(APP->window->loadSvg(asset::plugin(pluginInstance,"res/Knob_28.svg")));
};

BlueSnapMLKnob::BlueSnapMLKnob() {
    setSVG(APP->window->loadSvg(asset::plugin(pluginInstance,"res/Knob.svg")));
	snap = true;
};

SmallBlueSnapMLKnob::SmallBlueSnapMLKnob() {
    setSVG(APP->window->loadSvg(asset::plugin(pluginInstance,"res/Knob_28.svg")));
	snap = true;
};


RedMLKnob::RedMLKnob() {
    setSVG(APP->window->loadSvg(asset::plugin(pluginInstance,"res/RedKnob.svg")));
};

SmallRedMLKnob::SmallRedMLKnob() {
    setSVG(APP->window->loadSvg(asset::plugin(pluginInstance,"res/RedKnob_28.svg")));
};

RedSnapMLKnob::RedSnapMLKnob() {
    setSVG(APP->window->loadSvg(asset::plugin(pluginInstance,"res/RedKnob.svg")));
	snap = true;
};

SmallRedSnapMLKnob::SmallRedSnapMLKnob() {
    setSVG(APP->window->loadSvg(asset::plugin(pluginInstance,"res/RedKnob_28.svg")));
	snap = true;
};



GreyMLKnob::GreyMLKnob() {
    setSVG(APP->window->loadSvg(asset::plugin(pluginInstance,"res/GreyKnob.svg")));
};

SmallGreyMLKnob::SmallGreyMLKnob() {
    setSVG(APP->window->loadSvg(asset::plugin(pluginInstance,"res/GreyKnob_28.svg")));
};


GreySnapMLKnob::GreySnapMLKnob() {
    setSVG(APP->window->loadSvg(asset::plugin(pluginInstance,"res/GreyKnob.svg")));
	snap = true;
};

SmallGreySnapMLKnob::SmallGreySnapMLKnob() {
    setSVG(APP->window->loadSvg(asset::plugin(pluginInstance,"res/GreyKnob_28.svg")));
	snap = true;
};



MLPort::MLPort() {
	setSVG(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Jack.svg")));
};


MLButton::MLButton() {
	addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/MLButton_0.svg")));
	addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/MLButton_1.svg")));
	sw->wrap();
	box.size = sw->box.size;
};


MLSmallButton::MLSmallButton() {
	addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/SmallButton_0.svg")));
	addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/SmallButton_1.svg")));
	sw->wrap();
	box.size = sw->box.size;
};


ML_LEDButton::ML_LEDButton() {

	addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/LEDButton.svg")));
	sw->wrap();
	box.size = sw->box.size;

	shadow->box.size = box.size;
	shadow->blurRadius = 0.0f;
	shadow->box.pos = Vec(0, sw->box.size.y * 0.1);
	
};

ML_MediumLEDButton::ML_MediumLEDButton() {

	addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/LEDButton_medium.svg")));
	sw->wrap();
	box.size = sw->box.size;

	shadow->box.size = box.size;
	shadow->blurRadius = 0.0f;
	shadow->box.pos = Vec(0, sw->box.size.y * 0.1);
	
};


ML_SmallLEDButton::ML_SmallLEDButton() {

	addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/LEDButton_small.svg")));
	sw->wrap();
	box.size = sw->box.size;

	shadow->box.size = box.size;
	shadow->blurRadius = 0.0f;
	shadow->box.pos = Vec(0, sw->box.size.y * 0.1);
	
};




ML_ResetButton::ML_ResetButton() {

	addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ResetButton_0.svg")));
	addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ResetButton_1.svg")));

	sw->wrap();
	box.size = sw->box.size;
}

MLSwitch::MLSwitch() {

	addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Switch_0.svg")));
	addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Switch_1.svg")));
	addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Switch_2.svg")));

	shadow->box.size = box.size;
	shadow->blurRadius = 0.0f;
	shadow->box.pos = Vec(0, sw->box.size.y * 0.1);

};

MLSwitch2::MLSwitch2() {
	addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Switch_0.svg")));
	addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Switch_2.svg")));

	shadow->box.size = box.size;
	shadow->blurRadius = 0.0f;
	shadow->box.pos = Vec(0, sw->box.size.y * 0.1);
    
};

BlueMLSwitch::BlueMLSwitch() {
	addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/BlueSwitch_0.svg")));
	addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/BlueSwitch_1.svg")));
	addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/BlueSwitch_2.svg")));

   	shadow->box.size = box.size;
	shadow->blurRadius = 0.0f;
	shadow->box.pos = Vec(0, sw->box.size.y * 0.1);

};
