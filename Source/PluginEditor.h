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
    typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;

    MidiWahAudioProcessorEditor(MidiWahAudioProcessor&, ParameterHelper&, MidiKeyboardState&);

    ~MidiWahAudioProcessorEditor();

    //==============================================================================
    void paint(Graphics&) override;

    void resized() override;

    //==============================================================================
    void setupSourceToggles();

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MidiWahAudioProcessor& processor;

    ParameterHelper& parameterHelper;
    MidiKeyboardState& keyboardState;

    MidiKeyboardComponent keyboard;

    Slider centerFreqSlider;
    std::unique_ptr<SliderAttachment> centerFreqAttachment;
    Label centerFreqLabel;

    Slider qSlider;
    std::unique_ptr<SliderAttachment> qAttachment;
    Label qLabel;

    Slider gainSlider;
    std::unique_ptr<SliderAttachment> gainAttachment;
    Label gainLabel;

    Slider driveSlider;
    std::unique_ptr<SliderAttachment> driveAttachment;
    Label driveLabel;

    /*ComboBox ladderType;
    std::unique_ptr<ComboBoxAttachment> ladderTypeAttachment;*/

    // -----
    typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

    DrawablePath bgPath;
    DrawablePath borderPath;
    Label nameLabel;

    GroupComponent freqGroup;

    enum RadioButtonIds
    {
        frequencySourceButtons = 1001
    };

    ToggleButton midiSourceButton;
    std::unique_ptr<ButtonAttachment> freqToggleAttachment;

    ToggleButton sliderSourceButton;

    // Inherited via MidiKeyboardStateListener
    void handleNoteOn(MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity) override;

    void handleNoteOff(MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity) override;

    // -----

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiWahAudioProcessorEditor)
};
