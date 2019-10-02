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
class MyMidiKeyboardComponent : public MidiKeyboardComponent
{
public:
    MyMidiKeyboardComponent(OvertoneFilterAudioProcessor& p, MidiKeyboardState& state,
                            Orientation orientation);

    ~MyMidiKeyboardComponent();

protected:
    bool mouseDraggedToKey(int midiNoteNumber, const MouseEvent& e) override;

    bool mouseDownOnKey(int midiNoteNumber, const MouseEvent& e) override;

    void mouseUpOnKey(int midiNoteNumber, const MouseEvent& e) override;

private:
    OvertoneFilterAudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MyMidiKeyboardComponent)
};
