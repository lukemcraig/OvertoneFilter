#include "../JuceLibraryCode/JuceHeader.h"

#pragma once
class MyBandPass : public IIRFilter
{
public:
	void makeMyBandPass(const double inverseSampleRate, const double centerFreq, const double Q, const double gain) noexcept;
	void copyCoefficientsFrom(const MyBandPass & other) noexcept;
private:
	//==============================================================================
	JUCE_LEAK_DETECTOR(MyBandPass);	
};

