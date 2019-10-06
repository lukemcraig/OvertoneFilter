#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "MyMidiKeyboardComponent.h"
#include "LevelMeter.h"

//==============================================================================
/**
*/
class OvertoneFilterEditor : public AudioProcessorEditor
{
public:

    OvertoneFilterEditor(OvertoneFilterAudioProcessor&, ParameterHelper&, MidiKeyboardState&, LevelMeterAudioSource&,
                         LevelMeterAudioSource&, LevelMeterAudioSource&);

    ~OvertoneFilterEditor();

    //==============================================================================
    void paint(Graphics&) override;

    void resized() override;

    //==============================================================================
    void makeLabelUpperCase(Label& label);

private:
    typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;

    OvertoneFilterAudioProcessor& processor;
    ParameterHelper& parameterHelper;
    MidiKeyboardState& keyboardState;

    MyMidiKeyboardComponent keyboard;

    Slider standardSlider;
    std::unique_ptr<SliderAttachment> standardAttachment;
    Label standardLabel;

    Slider qSlider;
    std::unique_ptr<SliderAttachment> qAttachment;
    Label qLabel;

    Slider mixSlider;
    std::unique_ptr<SliderAttachment> mixAttachment;
    Label mixLabel;

    // -----
    Slider inputGainSlider;
    std::unique_ptr<SliderAttachment> inputGainAttachment;
    Label inputGainLabel;

    Slider wetGainSlider;
    std::unique_ptr<SliderAttachment> wetGainAttachment;
    Label wetGainLabel;

    Slider outputGainSlider;
    std::unique_ptr<SliderAttachment> outputGainAttachment;
    Label outputGainLabel;
    // -----

    LevelMeter inputMeter;
    LevelMeter wetMixMeter;
    LevelMeter outputMeter;

    Label inputMeterLabel;
    Label wetMixMeterLabel;
    Label outputMeterLabel;
    // -----
    DrawablePath bgPath;
    DrawablePath borderPath;
    Label nameLabel;
    // -----

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OvertoneFilterEditor)
};
