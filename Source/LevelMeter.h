/*
  ==============================================================================

    LevelMeter.h
    Created: 2 Oct 2019 3:52:35pm
    Author:  Luke

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "LevelMeterAudioSource.h"
#include "Shape.h"

//==============================================================================
/*
*/
class LevelMeter : public Component
{
public:
    LevelMeter(LevelMeterAudioSource&, OpenGLContext&, Colour c = Colours::red, int numLEDs = 5);

    ~LevelMeter();

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
    LevelMeterAudioSource& levelMeterAudioSource;
    Colour clipColour;
    int numLEDs;

    //==============================================================================
    struct Vertex
    {
        float position[3];
        float normal[3];
        float colour[4];
        float texCoord[2];
    };

    //==============================================================================
    struct Uniforms
    {
        Uniforms(OpenGLContext& openGLContext, OpenGLShaderProgram& shaderProgram);

        std::unique_ptr<OpenGLShaderProgram::Uniform>
            iResolution, iTime, slider0, iChannel0, iChannel1, iFrame, iSpectrum, iViewport;

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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LevelMeter)
};
