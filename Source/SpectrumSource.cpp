/*
  ==============================================================================

    SpectrumSource.cpp
    Created: 11 Oct 2019 12:53:12pm
    Author:  Luke

  ==============================================================================
*/

#include "SpectrumSource.h"

SpectrumSource::SpectrumSource() : forwardFFT(fftOrder), window(fftSize, dsp::WindowingFunction<float>::hann)
{
}

SpectrumSource::~SpectrumSource()
{
}

bool SpectrumSource::getSpectrum(Image& spectrumImage, int row)
{
    if (nextFFTBlockReady)
    {
        window.multiplyWithWindowingTable(fftData.data(), fftSize);
        forwardFFT.performFrequencyOnlyForwardTransform(fftData.data());

        for (int i = 0; i < fftSizePositive; ++i)
        {
            auto value = fftData[i];

            value = jmap(jlimit(mindB, maxdB, Decibels::gainToDecibels(value)
                                - Decibels::gainToDecibels(static_cast<float>(fftSize))),
                         mindB, maxdB, 0.0f, 1.0f);

            auto pixelValue = static_cast<uint8>(jmin((value) * 255.0f, 255.0f));
                        
            spectrumImage.setPixelAt(i, row, Colour(pixelValue, pixelValue, pixelValue, pixelValue));
        }
        nextFFTBlockReady = false;
        return true;
    }
    return false;
}

void SpectrumSource::pushSample(float sample)
{
    if (fifoIndex % hopSize == 0)
    {
        if (! nextFFTBlockReady)
        {
            auto fifoRead = fifoIndex;
            for (int i = 0; i < fftSize; ++i)
            {
                if (fifoRead == fftSize)
                    fifoRead = 0;
                fftData[i] = fifo[fifoRead];
                ++fifoRead;
            }
            nextFFTBlockReady = true;
        }
        if (fifoIndex == fftSize)
            fifoIndex = 0;
    }
    fifo[fifoIndex] = sample;
    ++fifoIndex;
}
