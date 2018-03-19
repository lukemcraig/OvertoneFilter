/*
  ==============================================================================

	This file was auto-generated!

	It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


//==============================================================================
/**
*/
class MidiWahAudioProcessorEditor : public AudioProcessorEditor, public AudioProcessorValueTreeState::Listener
{
public:
	typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

	MidiWahAudioProcessorEditor(MidiWahAudioProcessor&, AudioProcessorValueTreeState&);
	~MidiWahAudioProcessorEditor();

	//==============================================================================
	void paint(Graphics&) override;
	void resized() override;
	void parameterChanged(const String &parameterID, float newValue) override;

private:
	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	MidiWahAudioProcessor & processor;

	AudioProcessorValueTreeState& valueTreeState;

	Slider centerFreqSlider_;
	std::unique_ptr<SliderAttachment> centerFreqAttachment_;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiWahAudioProcessorEditor)
};
