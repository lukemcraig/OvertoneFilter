/*
  ==============================================================================

	This file was auto-generated!

	It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MidiWahAudioProcessorEditor::MidiWahAudioProcessorEditor(MidiWahAudioProcessor& p, AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor(&p), processor(p), valueTreeState(vts)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(400, 300);
    centerFreqSlider_.setSliderStyle(Slider::LinearHorizontal);
    centerFreqSlider_.setTextBoxStyle(Slider::TextBoxLeft, false, 120, centerFreqSlider_.getTextBoxHeight());
    centerFreqSlider_.setPopupDisplayEnabled(true, false, this);
    centerFreqSlider_.setTextValueSuffix("Hz");

    addAndMakeVisible(&centerFreqSlider_);
    centerFreqAttachment_.reset(new SliderAttachment(valueTreeState, processor.PID_CENTERFREQ, centerFreqSlider_));
    valueTreeState.addParameterListener(processor.PID_CENTERFREQ, this);

    addAndMakeVisible(&qSlider_);
    qAttachment_.reset(new SliderAttachment(valueTreeState, processor.PID_Q, qSlider_));
    valueTreeState.addParameterListener(processor.PID_Q, this);

    addAndMakeVisible(&gainSlider_);
    gainAttachment_.reset(new SliderAttachment(valueTreeState, processor.PID_GAIN, gainSlider_));
    valueTreeState.addParameterListener(processor.PID_GAIN, this);

    addAndMakeVisible(&driveSlider_);
    driveAttachment_.reset(new SliderAttachment(valueTreeState, processor.PID_DRIVE, driveSlider_));
    valueTreeState.addParameterListener(processor.PID_DRIVE, this);

    ladderType_.addItem("LP12", 1);
    ladderType_.addItem("LP24", 2);
    ladderType_.addItem("HP12", 3);
    ladderType_.addItem("HP24", 4);
    addAndMakeVisible(&ladderType_);
    ladderTypeAttachment_.reset(new ComboBoxAttachment(valueTreeState, processor.PID_LADDER_TYPE, ladderType_));
    valueTreeState.addParameterListener(processor.PID_LADDER_TYPE, this);

    startTimerHz(30);
}

MidiWahAudioProcessorEditor::~MidiWahAudioProcessorEditor()
{
    valueTreeState.removeParameterListener(processor.PID_CENTERFREQ, this);

    valueTreeState.removeParameterListener(processor.PID_Q, this);
    valueTreeState.removeParameterListener(processor.PID_GAIN, this);
    valueTreeState.removeParameterListener(processor.PID_DRIVE, this);
    valueTreeState.removeParameterListener(processor.PID_LADDER_TYPE, this);
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
    const auto midiVisible = *valueTreeState.getRawParameterValue(pidToggleMidiSource);
    if (midiVisible == 1.0f)
        nPanes += 2;
    const auto paneAreaHeight = area.getHeight() / nPanes;
    const auto paneMargin = 5;
    {
        auto depthAndFLabelsArea = area.removeFromTop(paneAreaHeight).reduced(paneMargin);
        auto fLabelsArea = depthAndFLabelsArea.removeFromRight(300);
        layoutFLabels(fLabelsArea);
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
    if (parameterID == processor.PID_Q)
    {
        processor.updateFilters();
    }
    else if (parameterID == processor.PID_GAIN)
    {
        processor.updateFilters();
    }
    else if (parameterID == processor.PID_DRIVE)
    {
        processor.updateFilters();
    }
    else if (parameterID == processor.PID_LADDER_TYPE)
    {
        processor.updateFilterType();
    }
}

void MidiWahAudioProcessorEditor::timerCallback()
{
    centerFreqSlider_.setValue(processor.midiDebugNumber_);
}
