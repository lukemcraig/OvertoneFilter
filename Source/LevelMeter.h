/*
  ==============================================================================

    LevelMeter.h
    Created: 2 Oct 2019 3:52:35pm
    Author:  Luke McDuffie Craig

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
    LevelMeter(LevelMeterAudioSource&, OpenGLContext&);

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
    struct Uniforms
    {
        Uniforms(OpenGLContext& openGLContext, OpenGLShaderProgram& shaderProgram);

        std::unique_ptr<OpenGLShaderProgram::Uniform>
            iResolution, iViewport, iLevel;

    private:
        static OpenGLShaderProgram::Uniform* createUniform(OpenGLContext& openGLContext,
                                                           OpenGLShaderProgram& shaderProgram,
                                                           const char* uniformName);
    };

    //==============================================================================

    LevelMeterAudioSource& levelMeterAudioSource;
    OpenGLContext& openGLContext;

    String vertexShader;
    String fragmentShader;

    std::unique_ptr<OpenGLShaderProgram> shaderProgram;
    std::unique_ptr<Shape> quad;
    std::unique_ptr<Attributes> attributes;
    std::unique_ptr<Uniforms> uniforms;

    //==============================================================================

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LevelMeter)
};
