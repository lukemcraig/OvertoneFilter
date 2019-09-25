#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class MidiWahAudioProcessorEditor : public AudioProcessorEditor,
                                    public AudioProcessorValueTreeState::Listener,
                                    private MidiKeyboardStateListener,
                                    private Timer
{
public:
    typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;

    MidiWahAudioProcessorEditor(MidiWahAudioProcessor&, ParameterHelper&, MidiKeyboardState&);

    ~MidiWahAudioProcessorEditor();

    //==============================================================================
    void paint(Graphics&) override;

    void resized() override;

    void parameterChanged(const String& parameterID, float newValue) override;

    void timerCallback() override;

    //==============================================================================
    void setupSourceToggles();

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MidiWahAudioProcessor& processor;

    ParameterHelper& parameterHelper;

    Slider centerFreqSlider;
    std::unique_ptr<SliderAttachment> centerFreqAttachment;

    Slider qSlider;
    std::unique_ptr<SliderAttachment> qAttachment;

    Slider gainSlider;
    std::unique_ptr<SliderAttachment> gainAttachment;

    Slider driveSlider;
    std::unique_ptr<SliderAttachment> driveAttachment;

    ComboBox ladderType;
    std::unique_ptr<ComboBoxAttachment> ladderTypeAttachment;

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

    MidiKeyboardComponent keyboard;
    MidiKeyboardState& keyboardState;   

    // Inherited via MidiKeyboardStateListener
    void handleNoteOn(MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity) override;

    void handleNoteOff(MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity) override;

    // -----

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiWahAudioProcessorEditor)
};
