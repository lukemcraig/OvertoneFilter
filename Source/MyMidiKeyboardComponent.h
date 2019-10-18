/*
  ==============================================================================

    MyMidiKeyboardComponent.h
    Created: 1 Oct 2019 4:28:55pm
    Author:  Luke McDuffie Craig

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

//==============================================================================
/*
*/
class MyMidiKeyboardComponent : public MidiKeyboardComponent,
                                public AudioProcessorValueTreeState::Listener,
                                public Timer
{
public:
    MyMidiKeyboardComponent(OvertoneFilterAudioProcessor& p, MidiKeyboardState& state,
                            Orientation orientation, ParameterHelper& ph);

    ~MyMidiKeyboardComponent();

    void paint(Graphics&) override;

    void parameterChanged(const String& parameterID, float newValue) override;

    void mouseUp(const MouseEvent&) override;

    void timerCallback() override;

protected:
    bool mouseDraggedToKey(int midiNoteNumber, const MouseEvent& e) override;

    bool mouseDownOnKey(int midiNoteNumber, const MouseEvent& e) override;

    void drawWhiteNote(int midiNoteNumber, Graphics& g, Rectangle<float> area, bool isDown, bool isOver,
                       Colour lineColour, Colour textColour) override;

    void drawBlackNote(int midiNoteNumber, Graphics& g, Rectangle<float> area, bool isDown, bool isOver,
                       Colour noteFillColour) override;

private:
    OvertoneFilterAudioProcessor& processor;
    ParameterHelper& parameterHelper;
    int currentNoteDown = -1;
    bool needToRepaint{};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MyMidiKeyboardComponent)
};
