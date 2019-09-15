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
class MidiWahAudioProcessorEditor : public AudioProcessorEditor,
                                    public AudioProcessorValueTreeState::Listener,
                                    private Timer
{
public:
    typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;

    MidiWahAudioProcessorEditor(MidiWahAudioProcessor&, AudioProcessorValueTreeState&);

    ~MidiWahAudioProcessorEditor();

    //==============================================================================
    void paint(Graphics&) override;

    void resized() override;

    void parameterChanged(const String& parameterID, float newValue) override;

    void timerCallback() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MidiWahAudioProcessor& processor;

    AudioProcessorValueTreeState& valueTreeState;

    Slider centerFreqSlider_;
    std::unique_ptr<SliderAttachment> centerFreqAttachment_;

    Slider qSlider_;
    std::unique_ptr<SliderAttachment> qAttachment_;

    Slider gainSlider_;
    std::unique_ptr<SliderAttachment> gainAttachment_;

    Slider driveSlider_;
    std::unique_ptr<SliderAttachment> driveAttachment_;

    ComboBox ladderType_;
    std::unique_ptr<ComboBoxAttachment> ladderTypeAttachment_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiWahAudioProcessorEditor)
};
