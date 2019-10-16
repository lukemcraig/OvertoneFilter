/*
  ==============================================================================

    SpectrumSource.h
    Created: 11 Oct 2019 12:53:12pm
    Author:  Luke

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class SpectrumSource
{
public:
    SpectrumSource();

    ~SpectrumSource();

    bool getSpectrum(Image& spectrumImage, int row);

    void pushSample(float sample);

    void setSampleRate(double newSampleRate) { sampleRate = newSampleRate; }

    double getSampleRate() { return sampleRate; }

    enum
    {
        fftOrder = 12,
        fftSize = 1 << fftOrder,
        hopSize = fftSize / 2,
        fftSizePositive = 1 << (fftOrder - 1)
    };

    dsp::FFT forwardFFT;
    dsp::WindowingFunction<float> window;
    std::array<float, fftSize> fifo{};
    std::array<float, 2 * fftSize> fftData{};
    int fifoIndex = 0;
    bool nextFFTBlockReady = false;

private:
    float mindB = -100.0f;
    float maxdB = 0.0f;
    double sampleRate{};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrumSource)
};
