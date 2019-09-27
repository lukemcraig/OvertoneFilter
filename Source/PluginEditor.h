#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class MidiWahAudioProcessorEditor : public AudioProcessorEditor,
                                    private MidiKeyboardStateListener
{
public:
    MidiWahAudioProcessorEditor(MidiWahAudioProcessor&, ParameterHelper&, MidiKeyboardState&);

    ~MidiWahAudioProcessorEditor();

    //==============================================================================
    void paint(Graphics&) override;

    void resized() override;

private:
    typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;

    MidiWahAudioProcessor& processor;
    ParameterHelper& parameterHelper;
    MidiKeyboardState& keyboardState;

    MidiKeyboardComponent keyboard;

    Slider qSlider;
    std::unique_ptr<SliderAttachment> qAttachment;
    Label qLabel;

    Slider gainSlider;
    std::unique_ptr<SliderAttachment> gainAttachment;
    Label gainLabel;

    Slider wetDrySlider;
    std::unique_ptr<SliderAttachment> wetDryAttachment;
    Label wetDryLabel;

    // -----
    typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

    DrawablePath bgPath;
    DrawablePath borderPath;
    Label nameLabel;

    GroupComponent freqGroup;

    // Inherited via MidiKeyboardStateListener
    void handleNoteOn(MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity) override;

    void handleNoteOff(MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity) override;

    // -----

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiWahAudioProcessorEditor)
};
