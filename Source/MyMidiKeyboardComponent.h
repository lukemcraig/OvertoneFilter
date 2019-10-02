/*
  ==============================================================================

    MyMidiKeyboardComponent.h
    Created: 1 Oct 2019 4:28:55pm
    Author:  Luke

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

//==============================================================================
/*
*/
class MyMidiKeyboardComponent : public MidiKeyboardComponent
{
public:
    MyMidiKeyboardComponent(MidiWahAudioProcessor& p, MidiKeyboardState& state,
                            Orientation orientation);

    ~MyMidiKeyboardComponent();

protected:
    bool mouseDraggedToKey(int midiNoteNumber, const MouseEvent& e) override;

    bool mouseDownOnKey(int midiNoteNumber, const MouseEvent& e) override;

    void mouseUpOnKey(int midiNoteNumber, const MouseEvent& e) override;

private:
    MidiWahAudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MyMidiKeyboardComponent)
};