/*
  ==============================================================================

    SpectrumDisplay.h
    Created: 9 Oct 2019 12:19:27am
    Author:  Luke

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "Shape.h"

//==============================================================================
/*
*/
class SpectrumDisplay : public Component
{
public:
    SpectrumDisplay(OvertoneFilterAudioProcessor&, OpenGLContext&, SpectrumSource&, SpectrumSource&, ParameterHelper&);

    ~SpectrumDisplay();

    void paint(Graphics&) override;

    void resized() override;

    //==============================================================================
    void initialiseOpenGL();

    void shutdown();

    void renderScene();

    void render();

    void createShaders();

    void renderOpenGL();

    //==============================================================================

private:
    OvertoneFilterAudioProcessor& processor;
    ParameterHelper& parameterHelper;
    SpectrumSource& inputSpectrumSource;
    SpectrumSource& outputSpectrumSource;
    OpenGLTexture spectrumTexture;
    Image spectrumImage = Image(Image::PixelFormat::ARGB, SpectrumSource::fftSizePositive, 2, true);

    //==============================================================================
    struct Uniforms
    {
        Uniforms(OpenGLContext& openGLContext, OpenGLShaderProgram& shaderProgram);

        std::unique_ptr<OpenGLShaderProgram::Uniform>
            iResolution, iTime, slider0, iChannel0, iChannel1, iFrame, iSpectrum, iViewport, iPitchStandard;

    private:
        static OpenGLShaderProgram::Uniform* createUniform(OpenGLContext& openGLContext,
                                                           OpenGLShaderProgram& shaderProgram,
                                                           const char* uniformName);
    };

    String vertexShader;
    String fragmentShader;

    std::unique_ptr<OpenGLShaderProgram> shaderProgram;
    std::unique_ptr<Shape> quad;
    std::unique_ptr<Attributes> attributes;
    std::unique_ptr<Uniforms> uniforms;

    //==============================================================================

    /** The GL context */
    OpenGLContext& openGLContext;
    int frameCounter{};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrumDisplay)
};
