#include "JuceHeader.h"

class TextSlider : public Component
{
public:
	const float PANEL_NAME_FONT_SIZE = 24.0f;
	const float PARAM_LABEL_FONT_SIZE = 18.0f;
	const int PANEL_NAME_HEIGHT = 32;
	const int LOCAL_MARGIN = 2;
	const int LABEL_WIDTH = 80;
	const Font panelNameFont() { return Font(PANEL_NAME_FONT_SIZE, Font::plain).withTypefaceStyle("Italic"); };
	const Font paramLabelFont() { return Font(PARAM_LABEL_FONT_SIZE, Font::plain).withTypefaceStyle("Regular"); };

	Slider slider;
	Label label;

	TextSlider(std::string labelName, std::string unit, float value, float start, float end, Slider::Listener *listener, float degree = 0.1f, float pivot = NULL)
		:slider(Slider::SliderStyle::LinearHorizontal, Slider::TextEntryBoxPosition::TextBoxLeft)
	{
		slider.setRange(start, end, degree);
		slider.setValue(value, dontSendNotification);
		slider.setTextValueSuffix(std::string(" ") + unit);
		slider.addListener(listener);

		if (pivot != NULL)
		{
			slider.setSkewFactorFromMidPoint(pivot);
		}

		label.setText(labelName, dontSendNotification);
		label.setFont(Font(PARAM_LABEL_FONT_SIZE, Font::plain).withTypefaceStyle("Regular"));
		label.setJustificationType(Justification::centred);
		label.setEditable(false, false, false);

		addAndMakeVisible(slider);
		addAndMakeVisible(label);
	};

	TextSlider(std::string labelName, std::string unit, AudioParameterFloat *param, Slider::Listener *listener, float degree = 0.1f, float pivot = NULL)
		: TextSlider(labelName, unit, param->get(), param->range.start, param->range.end, listener, degree, pivot)
	{};
	TextSlider(std::string labelName, std::string unit, AudioParameterInt *param, Slider::Listener *listener)
		: TextSlider(labelName, unit, param->get(), param->getRange().getEnd(), param->getRange().getEnd(), listener, 1.0f)
	{};

	virtual void setAlpha(float alpha)
	{
		slider.setAlpha(alpha);
		label.setAlpha(alpha);
	};

	virtual void resized() override
	{
		Rectangle<int> bounds = getLocalBounds();
		label.setBounds(bounds.removeFromLeft(LABEL_WIDTH).reduced(LOCAL_MARGIN));
		slider.setBounds(bounds.reduced(LOCAL_MARGIN));
	};

	virtual void setValue(float val)
	{
		slider.setValue(val, dontSendNotification);
	};

	virtual float getValue()
	{
		return slider.getValue();
	};

	virtual void addListener(Slider::Listener *listener)
	{
		slider.addListener(listener);
	};

private:
	TextSlider();
};

class TextSliderIncDec : public TextSlider
{
public:

	TextSliderIncDec(std::string labelName, std::string unit, int value, int start, int end, Slider::Listener *listener)
		: TextSlider(labelName, unit, value, start, end, listener, 1.0f)
	{
		slider.setSliderStyle(Slider::SliderStyle::IncDecButtons);
	};

	TextSliderIncDec(std::string labelName, std::string unit, AudioParameterInt *param, Slider::Listener *listener)
		: TextSliderIncDec(labelName, unit, param->get(), param->getRange().getStart(), param->getRange().getEnd(), listener)
	{};

private:
	TextSliderIncDec();
};

class TextSelector : public Component
{
public:
	const float PANEL_NAME_FONT_SIZE = 24.0f;
	const float PARAM_LABEL_FONT_SIZE = 18.0f;
	const int PANEL_NAME_HEIGHT = 32;
	const int LOCAL_MARGIN = 2;
	const int LABEL_WIDTH = 80;
	const Font panelNameFont() { return Font(PANEL_NAME_FONT_SIZE, Font::plain).withTypefaceStyle("Italic"); };
	const Font paramLabelFont() { return Font(PARAM_LABEL_FONT_SIZE, Font::plain).withTypefaceStyle("Regular"); };

	ComboBox selector;
	Label label;

	TextSelector(std::string labelName, AudioParameterChoice *paramList, ComboBox::Listener *listener)
		:selector(labelName)
	{
		selector.addItemList(paramList->getAllValueStrings(), 1);
		selector.setSelectedItemIndex(paramList->getIndex(), dontSendNotification);
		selector.setJustificationType(Justification::centred);
		selector.addListener(listener);
		addAndMakeVisible(selector);

		label.setFont(paramLabelFont());
		label.setText(labelName, dontSendNotification);
		label.setJustificationType(Justification::centred);
		label.setEditable(false, false, false);
		addAndMakeVisible(label);
	};

	virtual void resized() override
	{
		Rectangle<int> bounds = getLocalBounds();
		label.setBounds(bounds.removeFromLeft(LABEL_WIDTH).reduced(LOCAL_MARGIN));
		selector.setBounds(bounds.reduced(LOCAL_MARGIN * 2));
	};

	virtual void addListener(ComboBox::Listener *listener)
	{
		selector.addListener(listener);
	};

	virtual float getSelectedItemIndex()
	{
		return selector.getSelectedItemIndex();
	};

	virtual void  setSelectedItemIndex(int index)
	{
		selector.setSelectedItemIndex(index, dontSendNotification);
	}

private:
	TextSelector();
};

class SwitchButton : public Component
{
public:
	const float PANEL_NAME_FONT_SIZE = 24.0f;
	const float PARAM_LABEL_FONT_SIZE = 18.0f;
	const std::int32_t PANEL_NAME_HEIGHT = 32;
	const std::int32_t LOCAL_MARGIN = 2;
	const std::int32_t LABEL_WIDTH = 80;
	const Font panelNameFont() { return Font(PANEL_NAME_FONT_SIZE, Font::plain).withTypefaceStyle("Italic"); };
	const Font paramLabelFont() { return Font(PARAM_LABEL_FONT_SIZE, Font::plain).withTypefaceStyle("Regular"); };

	ToggleButton button;

	SwitchButton(std::string label, AudioParameterBool *param, ToggleButton::Listener *listener)
	{
		button.setButtonText("ON");
		button.setToggleState(param->get(), dontSendNotification);
		button.addListener(listener);
		addAndMakeVisible(button);
	};

	virtual void resized() override
	{
		Rectangle<int> bounds = getLocalBounds();
		bounds.removeFromLeft(LABEL_WIDTH * 0.2);
		button.setBounds(bounds.reduced(LOCAL_MARGIN));
	};

	virtual void setToggleState(bool flag)
	{
		button.setToggleState(flag, dontSendNotification);
	};

	virtual bool getToggleState()
	{
		return button.getToggleState();
	}

	virtual void addListener(Button::Listener *listener)
	{
		button.addListener(listener);
	};

private:
	SwitchButton();
};

class PageButton : public Component
{
public:
	const float PANEL_NAME_FONT_SIZE = 24.0f;
	const float PARAM_LABEL_FONT_SIZE = 18.0f;
	const int PANEL_NAME_HEIGHT = 32;
	const int LOCAL_MARGIN = 2;
	const int LABEL_WIDTH = 80;
	const Font panelNameFont() { return Font(PANEL_NAME_FONT_SIZE, Font::plain).withTypefaceStyle("Italic"); };
	const Font paramLabelFont() { return Font(PARAM_LABEL_FONT_SIZE, Font::plain).withTypefaceStyle("Regular"); };

	TextButton button;

	PageButton(std::string labelName, Button::Listener *listener)
		:button(labelName)
	{
		button.addListener(listener);
		button.setColour(TextButton::ColourIds::buttonColourId, Colours::rebeccapurple);
		button.setColour(TextButton::ColourIds::buttonOnColourId, Colours::rebeccapurple.darker().darker());
		button.setColour(TextButton::ColourIds::textColourOffId, Colours::white);
		button.setColour(TextButton::ColourIds::textColourOnId, Colours::white);
		button.setToggleState(true, NotificationType::dontSendNotification);

		addAndMakeVisible(button);
	};
	virtual void resized() override
	{
		Rectangle<int> bounds = getLocalBounds();
		button.setBounds(bounds.reduced(LOCAL_MARGIN));
	};

	virtual void setToggleState(bool flag)
	{
		button.setToggleState(flag, NotificationType::dontSendNotification);
	};

private:
	PageButton();
};