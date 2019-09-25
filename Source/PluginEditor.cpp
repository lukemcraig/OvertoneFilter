/*
  ==============================================================================

	This file was auto-generated!

	It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MidiWahAudioProcessorEditor::MidiWahAudioProcessorEditor(MidiWahAudioProcessor& p, ParameterHelper& ph,
                                                         MidiKeyboardState& ks)
    : AudioProcessorEditor(&p), processor(p), parameterHelper(ph),
      keyboard(ks, MidiKeyboardComponent::horizontalKeyboard), keyboardState(ks)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(400, 300);
    centerFreqSlider.setSliderStyle(Slider::LinearHorizontal);
    centerFreqSlider.setTextBoxStyle(Slider::TextBoxLeft, false, 120, centerFreqSlider.getTextBoxHeight());
    centerFreqSlider.setPopupDisplayEnabled(true, false, this);
    centerFreqSlider.setTextValueSuffix("Hz");

    addAndMakeVisible(&centerFreqSlider);
    centerFreqAttachment.reset(new SliderAttachment(parameterHelper.valueTreeState, parameterHelper.PID_CENTERFREQ, centerFreqSlider));
    parameterHelper.valueTreeState.addParameterListener(parameterHelper.PID_CENTERFREQ, this);

    addAndMakeVisible(&qSlider);
    qAttachment.reset(new SliderAttachment(parameterHelper.valueTreeState, parameterHelper.PID_Q, qSlider));
    parameterHelper.valueTreeState.addParameterListener(parameterHelper.PID_Q, this);

    addAndMakeVisible(&gainSlider);
    gainAttachment.reset(new SliderAttachment(parameterHelper.valueTreeState, parameterHelper.PID_GAIN, gainSlider));
    parameterHelper.valueTreeState.addParameterListener(parameterHelper.PID_GAIN, this);

    addAndMakeVisible(&driveSlider);
    driveAttachment.reset(new SliderAttachment(parameterHelper.valueTreeState, parameterHelper.PID_DRIVE, driveSlider));
    parameterHelper.valueTreeState.addParameterListener(parameterHelper.PID_DRIVE, this);

    ladderType.addItem("LP12", 1);
    ladderType.addItem("LP24", 2);
    ladderType.addItem("HP12", 3);
    ladderType.addItem("HP24", 4);
    addAndMakeVisible(&ladderType);
    ladderTypeAttachment.reset(new ComboBoxAttachment(parameterHelper.valueTreeState, parameterHelper.PID_LADDER_TYPE, ladderType));
    parameterHelper.valueTreeState.addParameterListener(parameterHelper.PID_LADDER_TYPE, this);

    startTimerHz(30);

    // ----
    {
        borderPath.setFill(Colours::transparentBlack);
        borderPath.setStrokeType(PathStrokeType(1));
        borderPath.setStrokeFill(Colours::white);
        addAndMakeVisible(borderPath);
    }

    nameLabel.setText("Midi Wah - Luke M. Craig - " __DATE__ + String(" ") + __TIME__, dontSendNotification);
    addAndMakeVisible(nameLabel);

    //setupLfoFreqSlider();

    setupSourceToggles();

    //setupOffsets();

    //setupDepthSlider();

    //setupFLabels();

    addAndMakeVisible(keyboard);
    keyboardState.addListener(this);

    freqGroup.setText("Modulation Frequency:");
    freqGroup.setTextLabelPosition(Justification::centredLeft);
    addAndMakeVisible(freqGroup);

    startTimerHz(30);
    setResizable(true, true);
    setResizeLimits(400, 400, 1680, 1050);
    setSize(800, 600);
    // ----
}

void MidiWahAudioProcessorEditor::setupSourceToggles()
{
    midiSourceButton.setButtonText("Midi");
    addAndMakeVisible(midiSourceButton);
    freqToggleAttachment.reset(new ButtonAttachment(parameterHelper.valueTreeState, parameterHelper.pidToggleMidiSource,
                                                    midiSourceButton));
    midiSourceButton.setRadioGroupId(frequencySourceButtons);

    sliderSourceButton.setButtonText("Slider");
    addAndMakeVisible(sliderSourceButton);
    sliderSourceButton.setRadioGroupId(frequencySourceButtons);
    sliderSourceButton.setToggleState(!midiSourceButton.getToggleState(), dontSendNotification);
}

MidiWahAudioProcessorEditor::~MidiWahAudioProcessorEditor()
{
    parameterHelper.valueTreeState.removeParameterListener(parameterHelper.PID_CENTERFREQ, this);

    parameterHelper.valueTreeState.removeParameterListener(parameterHelper.PID_Q, this);
    parameterHelper.valueTreeState.removeParameterListener(parameterHelper.PID_GAIN, this);
    parameterHelper.valueTreeState.removeParameterListener(parameterHelper.PID_DRIVE, this);
    parameterHelper.valueTreeState.removeParameterListener(parameterHelper.PID_LADDER_TYPE, this);
}

//==============================================================================
void MidiWahAudioProcessorEditor::paint(Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

    g.setColour(Colours::white);
    g.setFont(15.0f);
}

void MidiWahAudioProcessorEditor::resized()
{
    {
        const auto bounds = getBounds().toFloat();
        RectanglePlacement placement(RectanglePlacement::fillDestination);
        const auto w = bgPath.getDrawableBounds().proportionOfWidth(0.17f);
        const auto fitTransform = placement.getTransformToFit(bgPath.getDrawableBounds().reduced(w), bounds);
        bgPath.setTransform(fitTransform.followedBy(AffineTransform::translation(0, 110.0f)));
    }

    auto area = getLocalBounds();
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
    auto nPanes = 2;
    //const auto midiVisible = *parameterHelper.valueTreeState.getRawParameterValue(pidToggleMidiSource);
    const auto midiVisible = 1.0f;
    if (midiVisible == 1.0f)
        nPanes += 2;
    const auto paneAreaHeight = area.getHeight() / nPanes;
    const auto paneMargin = 5;
    {
        auto depthAndFLabelsArea = area.removeFromTop(paneAreaHeight).reduced(paneMargin);
        //auto fLabelsArea = depthAndFLabelsArea.removeFromRight(300);
        // layoutFLabels(fLabelsArea);
        //auto depthArea = depthAndFLabelsArea.removeFromLeft(depthAndFLabelsArea.proportionOfWidth(0.5f));
        depthGroup.setBounds(depthAndFLabelsArea);
        depthSlider.setBounds(depthAndFLabelsArea.reduced(20, 20));
    }
    freqGroup.setBounds(area);
    {
        auto freqArea = area.removeFromTop(paneAreaHeight).reduced(paneMargin);
        freqArea.removeFromTop(20);
        auto freqAreaTop = freqArea.removeFromTop(20);
        lfoFreqSliderLabel.setBounds(freqAreaTop.removeFromLeft(60));
        midiSourceButton.setBounds(freqAreaTop.removeFromLeft(80));
        sliderSourceButton.setBounds(freqAreaTop.removeFromLeft(80));

        lfoFreqSlider.setBounds(freqArea.reduced(20, 10));
    }

    if (midiVisible == 1.0f)
    {
        auto offsetsArea = area.removeFromTop(paneAreaHeight).reduced(10);
        offsetsGroup.setBounds(offsetsArea);
        offsetsArea.reduce(10, 10);
        const auto nSliders = 4;
        const auto sliderWidth = offsetsArea.proportionOfWidth(1.0f / nSliders);
        offsetOctaveSlider.setBounds(offsetsArea.removeFromLeft(sliderWidth).reduced(20, 10));
        offsetSemitoneSlider.setBounds(offsetsArea.removeFromLeft(sliderWidth).reduced(20, 10));
        offsetCentsSlider.setBounds(offsetsArea.removeFromLeft(sliderWidth).reduced(20, 10));
        standardSlider.setBounds(offsetsArea.removeFromLeft(sliderWidth).reduced(20, 10));
    }

    {
        const auto keyboardArea = area.removeFromTop(paneAreaHeight).reduced(10, 10);
        keyboard.setBounds(keyboardArea);
    }
}

void MidiWahAudioProcessorEditor::parameterChanged(const String& parameterID, float newValue)
{
    //if (parameterID == processor.PID_CENTERFREQ) {
    //	processor.updateFilters();
    //}
    if (parameterID == parameterHelper.PID_Q)
    {
        processor.updateFilters();
    }
    else if (parameterID == parameterHelper.PID_GAIN)
    {
        processor.updateFilters();
    }
    else if (parameterID == parameterHelper.PID_DRIVE)
    {
        processor.updateFilters();
    }
    else if (parameterID == parameterHelper.PID_LADDER_TYPE)
    {
        processor.updateFilterType();
    }
}

void MidiWahAudioProcessorEditor::timerCallback()
{
    centerFreqSlider.setValue(processor.midiDebugNumber);
}

void MidiWahAudioProcessorEditor::handleNoteOn(MidiKeyboardState* source, int midiChannel, int midiNoteNumber,
                                               float velocity)
{
    // TODO
}

void MidiWahAudioProcessorEditor::handleNoteOff(MidiKeyboardState* source, int midiChannel, int midiNoteNumber,
                                                float velocity)
{
    // TODO
}
