/*
  ==============================================================================

    MySlider.cpp
    Created: 8 Oct 2019 1:22:11pm
    Author:  Luke

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "MySlider.h"

//==============================================================================
MySlider::MySlider()
{
}

MySlider::~MySlider()
{
}

void MySlider::mouseDown(const MouseEvent& e)
{
    Slider::mouseDown(e);
    // https://forum.juce.com/t/fr-make-the-bubble-component-used-for-the-slider-popup-display-customizable/33823/3
    if (auto* popupDisplay = getCurrentPopupDisplay())
        popupDisplay->setComponentEffect(nullptr);
}

void MySlider::mouseEnter(const MouseEvent& e)
{
    Slider::mouseEnter(e);
    // https://forum.juce.com/t/fr-make-the-bubble-component-used-for-the-slider-popup-display-customizable/33823/3
    if (auto* popupDisplay = getCurrentPopupDisplay())
        popupDisplay->setComponentEffect(nullptr);
}

void MySlider::mouseMove(const MouseEvent& e)
{
    Slider::mouseMove(e);
    // https://forum.juce.com/t/fr-make-the-bubble-component-used-for-the-slider-popup-display-customizable/33823/3
    if (auto* popupDisplay = getCurrentPopupDisplay())
        popupDisplay->setComponentEffect(nullptr);
}
