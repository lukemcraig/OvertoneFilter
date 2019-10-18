/*
  ==============================================================================

    SliderWithMeter.h
    Created: 15 Oct 2019 7:29:54pm
    Author:  Luke McDuffie Craig

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "Shape.h"
#include "ParameterHelper.h"

//==============================================================================
/*
*/
class SliderWithMeter : public Slider
{
public:
    SliderWithMeter(OpenGLContext&, ParameterHelper&);

    ~SliderWithMeter();

    void paint(Graphics&) override;

    void initialiseOpenGL();

    void shutdown();

    void renderScene();

    void render();

    void createShaders();

    void renderOpenGL();

    void mouseDown(const MouseEvent&) override;

    void mouseEnter(const MouseEvent&) override;

    void mouseMove(const MouseEvent&) override;

private:
    ParameterHelper& parameterHelper;
    //==============================================================================
    struct Uniforms
    {
        Uniforms(OpenGLContext& openGLContext, OpenGLShaderProgram& shaderProgram);

        std::unique_ptr<OpenGLShaderProgram::Uniform>
            iResolution, iViewport, iLevel, iSlider;

    private:
        static OpenGLShaderProgram::Uniform* createUniform(OpenGLContext& openGLContext,
                                                           OpenGLShaderProgram& shaderProgram,
                                                           const char* uniformName);
    };

    //==============================================================================
    OpenGLContext& openGLContext;

    String vertexShader;
    String fragmentShader;

    std::unique_ptr<OpenGLShaderProgram> shaderProgram;
    std::unique_ptr<Shape> quad;
    std::unique_ptr<Attributes> attributes;
    std::unique_ptr<Uniforms> uniforms;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SliderWithMeter)
};
