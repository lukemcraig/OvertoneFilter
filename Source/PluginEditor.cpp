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
}

MidiWahAudioProcessorEditor::~MidiWahAudioProcessorEditor()
{
	valueTreeState.removeParameterListener(processor.PID_CENTERFREQ, this);
	valueTreeState.removeParameterListener(processor.PID_Q, this);
	valueTreeState.removeParameterListener(processor.PID_GAIN, this);
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
	// This is generally where you'll want to lay out the positions of any
	// subcomponents in your editor..
	centerFreqSlider_.setBounds(30, 30, 300, 40);
	qSlider_.setBounds(30, centerFreqSlider_.getBottom(), 300, 40);
	gainSlider_.setBounds(30, qSlider_.getBottom(), 300, 40);
}

void MidiWahAudioProcessorEditor::parameterChanged(const String &parameterID, float newValue)
{
	if (parameterID == processor.PID_CENTERFREQ) {
		processor.updateFilters();
	}
	else if (parameterID == processor.PID_Q) {
		processor.updateFilters();
	}
	else if (parameterID == processor.PID_GAIN) {
		processor.updateFilters();
	}
}


