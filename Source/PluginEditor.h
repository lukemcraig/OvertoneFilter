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
    Slider lfoFreqSlider;
    std::unique_ptr<SliderAttachment> lfoFreqSliderAttachment;

    Label lfoFreqSliderLabel;

    GroupComponent offsetsGroup;

    Slider offsetOctaveSlider;
    std::unique_ptr<SliderAttachment> offsetOctaveSliderAttachment;

    Slider offsetSemitoneSlider;
    std::unique_ptr<SliderAttachment> offsetSemitoneSliderAttachment;

    Slider offsetCentsSlider;
    std::unique_ptr<SliderAttachment> offsetCentsSliderAttachment;

    Slider standardSlider;
    std::unique_ptr<SliderAttachment> standardSliderAttachment;

    GroupComponent fOutGroup;

    Label fLabel;
    Label fcLabel;

    static constexpr int nFLabels = 6;
    std::array<Label, nFLabels> freqLabels;

    Label fValueLabel;
    Label fcValueLabel;

    std::array<Label, nFLabels> freqValueLabels;

    enum RadioButtonIds
    {
        frequencySourceButtons = 1001
    };

    ToggleButton midiSourceButton;
    std::unique_ptr<ButtonAttachment> freqToggleAttachment;

    ToggleButton sliderSourceButton;

    Slider depthSlider;
    std::unique_ptr<SliderAttachment> depthSliderAttachment;
    GroupComponent depthGroup;

    MidiKeyboardComponent keyboard;
    MidiKeyboardState& keyboardState;

    static constexpr char const* noteNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};

    // Inherited via MidiKeyboardStateListener
    void handleNoteOn(MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity) override;

    void handleNoteOff(MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity) override;

    // -----

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiWahAudioProcessorEditor)
};
