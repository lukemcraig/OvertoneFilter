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
    OvertoneFilterEditor(OvertoneFilterAudioProcessor&, ParameterHelper&, MidiKeyboardState&, float&, int&);

    ~OvertoneFilterEditor();

    //==============================================================================
    void paint(Graphics&) override;

    void resized() override;

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

    Slider gainSlider;
    std::unique_ptr<SliderAttachment> gainAttachment;
    Label gainLabel;

    Slider wetDrySlider;
    std::unique_ptr<SliderAttachment> wetDryAttachment;
    Label wetDryLabel;

    // -----
    LevelMeter levelMeter;

    // -----

    DrawablePath bgPath;
    DrawablePath borderPath;
    Label nameLabel;
    // -----

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OvertoneFilterEditor)
};
