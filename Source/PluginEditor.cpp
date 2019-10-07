#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OvertoneFilterEditor::OvertoneFilterEditor(OvertoneFilterAudioProcessor& p,
                                           ParameterHelper& ph,
                                           MidiKeyboardState& ks,
                                           LevelMeterAudioSource& inputLevel,
                                           LevelMeterAudioSource& wetMixLevel,
                                           LevelMeterAudioSource& outputLevel)
    : AudioProcessorEditor(&p),
      processor(p),
      parameterHelper(ph),
      keyboardState(ks),
      keyboard(p, ks, MidiKeyboardComponent::horizontalKeyboard),
      inputMeter(inputLevel, Colours::blueviolet),
      wetMixMeter(wetMixLevel, Colours::blueviolet),
      outputMeter(outputLevel)
{
    const auto textEntryBoxWidth = 64;
    {
        standardSlider.setTextBoxStyle(Slider::TextBoxBelow, false, textEntryBoxWidth, 16);
        standardSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        addAndMakeVisible(standardSlider);
        standardAttachment.reset(new SliderAttachment(parameterHelper.valueTreeState,
                                                      parameterHelper.pidPitchStandard, standardSlider));

        standardLabel.setText("Pitch Standard", dontSendNotification);
        standardLabel.attachToComponent(&standardSlider, false);
        addAndMakeVisible(standardLabel);
    }
    {
        qSlider.setTextBoxStyle(Slider::NoTextBox, false, textEntryBoxWidth, 16);
        qSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        qSlider.setPopupDisplayEnabled(true, true, this);
        addAndMakeVisible(qSlider);
        qAttachment.reset(new SliderAttachment(parameterHelper.valueTreeState, parameterHelper.pidQ, qSlider));

        qLabel.setText("Resonance", dontSendNotification);
        qLabel.attachToComponent(&qSlider, false);
        addAndMakeVisible(qLabel);
    }
    {
        mixSlider.setTextBoxStyle(Slider::NoTextBox, false, textEntryBoxWidth, 16);
        mixSlider.setSliderStyle(Slider::LinearVertical);
        mixSlider.setPopupDisplayEnabled(true, true, this);
        addAndMakeVisible(mixSlider);
        mixAttachment.reset(new SliderAttachment(parameterHelper.valueTreeState, parameterHelper.pidMix,
                                                 mixSlider));

        mixLabel.setText("Mix", dontSendNotification);
        mixLabel.attachToComponent(&mixSlider, false);
        addAndMakeVisible(mixLabel);
    }

    {
        inputGainSlider.setTextBoxStyle(Slider::NoTextBox, false, textEntryBoxWidth, 16);
        inputGainSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        inputGainSlider.setPopupDisplayEnabled(true, true, this);
        addAndMakeVisible(inputGainSlider);
        inputGainAttachment.reset(new SliderAttachment(parameterHelper.valueTreeState, parameterHelper.pidInputGain,
                                                       inputGainSlider));
    }
    {
        wetGainSlider.setTextBoxStyle(Slider::NoTextBox, false, textEntryBoxWidth, 16);
        wetGainSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        wetGainSlider.setPopupDisplayEnabled(true, true, this);
        addAndMakeVisible(wetGainSlider);
        wetGainAttachment.reset(new SliderAttachment(parameterHelper.valueTreeState, parameterHelper.pidWetGain,
                                                     wetGainSlider));
    }
    {
        outputGainSlider.setTextBoxStyle(Slider::NoTextBox, false, textEntryBoxWidth, 16);
        outputGainSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        outputGainSlider.setPopupDisplayEnabled(true, true, this);
        addAndMakeVisible(outputGainSlider);
        outputGainAttachment.reset(new SliderAttachment(parameterHelper.valueTreeState, parameterHelper.pidOutputGain,
                                                        outputGainSlider));
    }

    {
        borderPath.setFill(Colours::transparentBlack);
        borderPath.setStrokeType(PathStrokeType(1));
        borderPath.setStrokeFill(Colours::white);
        addAndMakeVisible(borderPath);
    }
    {
        nameLabel.setText("Overtone Filter - Luke M. Craig - " __DATE__ + String(" ") + __TIME__, dontSendNotification);
        addAndMakeVisible(nameLabel);
    }
    {
        addAndMakeVisible(inputMeter);
        addAndMakeVisible(wetMixMeter);
        addAndMakeVisible(outputMeter);

        inputMeterLabel.setText("Dry", dontSendNotification);
        wetMixMeterLabel.setText("Wet", dontSendNotification);
        outputMeterLabel.setText("Output", dontSendNotification);

        inputMeterLabel.setJustificationType(Justification::centred);
        wetMixMeterLabel.setJustificationType(Justification::centred);
        outputMeterLabel.setJustificationType(Justification::centred);

        addAndMakeVisible(inputMeterLabel);
        addAndMakeVisible(wetMixMeterLabel);
        addAndMakeVisible(outputMeterLabel);
    }

    {
        makeLabelUpperCase(nameLabel);

        makeLabelUpperCase(standardLabel);
        makeLabelUpperCase(qLabel);
        makeLabelUpperCase(mixLabel);
        makeLabelUpperCase(inputGainLabel);
        makeLabelUpperCase(wetGainLabel);
        makeLabelUpperCase(outputGainLabel);

        makeLabelUpperCase(inputMeterLabel);
        makeLabelUpperCase(wetMixMeterLabel);
        makeLabelUpperCase(outputMeterLabel);
    }

    addAndMakeVisible(keyboard);
    setResizable(true, true);
    setResizeLimits(400, 400, 1680, 1050);
    setSize(1400, 400);
}

OvertoneFilterEditor::~OvertoneFilterEditor()
{
}

//==============================================================================
void OvertoneFilterEditor::paint(Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

    g.setColour(Colours::white);
    g.setFont(15.0f);
}

void OvertoneFilterEditor::resized()
{
    //TODO clean all this up
    {
        const auto bounds = getBounds().toFloat();
        RectanglePlacement placement(RectanglePlacement::fillDestination);
        const auto w = bgPath.getDrawableBounds().proportionOfWidth(0.17f);
        const auto fitTransform = placement.getTransformToFit(bgPath.getDrawableBounds().reduced(w), bounds);
        bgPath.setTransform(fitTransform.followedBy(AffineTransform::translation(0, 110.0f)));
    }

    auto area = getLocalBounds();
    DBG(area.getWidth());
    DBG(area.getHeight());
    // margins
    area.reduce(10, 10);

    {
        nameLabel.setJustificationType(Justification::centred);
        auto nameArea = area;
        nameLabel.setPaintingIsUnclipped(true);
        nameLabel.setBounds(nameArea.removeFromTop(5));
    }
    {
        auto pad = 10;
        auto w = -pad + (area.getWidth() - nameLabel.getFont().getStringWidthFloat(nameLabel.getText())) / 2.0f;
        Path path;
        path.startNewSubPath(area.getX() + w, area.getY());
        const auto topLeft = area.getTopLeft();
        path.lineTo(topLeft.getX(), topLeft.getY());
        const auto bottomLeft = area.getBottomLeft();
        path.lineTo(bottomLeft.getX(), bottomLeft.getY());
        const auto bottomRight = area.getBottomRight();
        path.lineTo(bottomRight.getX(), bottomRight.getY());
        const auto topRight = area.getTopRight();
        path.lineTo(topRight.getX(), topRight.getY());
        path.lineTo(topRight.getX() - w, topRight.getY());
        auto roundPath = path.createPathWithRoundedCorners(3);
        borderPath.setPath(roundPath);
    }
    area.reduce(10, 10);

    auto leftArea = area.removeFromLeft(area.proportionOfWidth(0.618));
    auto rightArea = area;

    // right area

    auto meterWidth = rightArea.proportionOfWidth(0.5);
    auto outputMeterArea = rightArea.removeFromRight(meterWidth);

    outputMeterArea.removeFromTop(outputMeterArea.getHeight() * 0.5 - meterWidth * 0.5);
    outputMeterLabel.setBounds(outputMeterArea.removeFromTop(16));
    outputGainSlider.setBounds(outputMeterArea.removeFromTop(meterWidth));
    outputMeter.setBounds(outputMeterArea.removeFromTop(32));

    mixSlider.setBounds(rightArea.removeFromRight(32).withTrimmedTop(16));

    auto wetMixMeterArea = rightArea.removeFromTop(rightArea.proportionOfHeight(0.5));
    auto inputMeterArea = rightArea;

    inputMeterLabel.setBounds(inputMeterArea.removeFromTop(16));
    inputMeter.setBounds(inputMeterArea.removeFromBottom(32));
    inputGainSlider.setBounds(inputMeterArea);

    wetMixMeterLabel.setBounds(wetMixMeterArea.removeFromTop(16));
    wetMixMeter.setBounds(wetMixMeterArea.removeFromBottom(32));
    wetGainSlider.setBounds(wetMixMeterArea);

    // left area

    const auto nPanes = 2;
    const auto paneAreaHeight = leftArea.getHeight() / nPanes;

    auto sliderArea = leftArea.removeFromTop(paneAreaHeight).reduced(10, 10);
    sliderArea.removeFromTop(16);

    const auto nSliders = 2;
    auto sliderHeight = sliderArea.getWidth() / nSliders;
    standardSlider.setBounds(sliderArea.removeFromLeft(sliderHeight));
    qSlider.setBounds(sliderArea.removeFromLeft(sliderHeight));

    const auto keyboardArea = leftArea.removeFromTop(paneAreaHeight).reduced(10, 0);
    keyboard.setBounds(keyboardArea);
}

//==============================================================================
void OvertoneFilterEditor::makeLabelUpperCase(Label& label)
{
    label.setText(label.getText().toUpperCase(), dontSendNotification);
}
