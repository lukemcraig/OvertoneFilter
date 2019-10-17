#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "MyMidiKeyboardComponent.h"
#include "LevelMeter.h"
#include "MySlider.h"
#include "SpectrumDisplay.h"
#include "Shape.h"
#include "SliderWithMeter.h"
#include "OvertoneLookAndFeel.h"

//==============================================================================
/**
*/
class OvertoneFilterEditor : public AudioProcessorEditor,
                             public OpenGLRenderer

{
public:

    OvertoneFilterEditor(OvertoneFilterAudioProcessor&, ParameterHelper&, MidiKeyboardState&, LevelMeterAudioSource&,
                         LevelMeterAudioSource&, LevelMeterAudioSource&, SpectrumSource&, SpectrumSource&);

    ~OvertoneFilterEditor();

    void makeLabelUpperCase(Label& label);

    //==============================================================================
    void paint(Graphics&) override;

    void setLabelAreaAboveCentered(Label& label, Rectangle<int>& labelArea);

    void resized() override;

    //==============================================================================
    void initialiseOpenGL();

    void shutdown();

    void shutdownOpenGL();

    void setupFBO();

    void renderToTexture();

    void renderScene();

    void render();

    void createShaders();

    void newOpenGLContextCreated() override;

    void renderOpenGL() override;

    void openGLContextClosing() override;

    //==============================================================================
private:
    typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;

    OvertoneLookAndFeel overtoneLookAndFeel;

    OvertoneFilterAudioProcessor& processor;
    ParameterHelper& parameterHelper;
    MidiKeyboardState& keyboardState;

    MyMidiKeyboardComponent keyboard;

    Slider standardSlider;
    std::unique_ptr<SliderAttachment> standardAttachment;
    Label standardLabel;

    MySlider qSlider;
    std::unique_ptr<SliderAttachment> qAttachment;
    Label qLabel;

    MySlider mixAttackSlider;
    std::unique_ptr<SliderAttachment> mixAttackAttachment;
    Label mixAttackLabel;

    MySlider mixReleaseSlider;
    std::unique_ptr<SliderAttachment> mixReleaseAttachment;
    Label mixReleaseLabel;

    // -----
    MySlider dryGainSlider;
    std::unique_ptr<SliderAttachment> inputGainAttachment;
    Label inputGainLabel;

    MySlider wetGainSlider;
    std::unique_ptr<SliderAttachment> wetGainAttachment;
    Label wetGainLabel;

    MySlider outputGainSlider;
    std::unique_ptr<SliderAttachment> outputGainAttachment;
    Label outputGainLabel;
    // -----

    LevelMeter dryMeter;
    LevelMeter wetMeter;
    LevelMeter outputMeter;

    LevelMeterAudioSource& outputLevel;

    Label dryMeterLabel;
    Label wetMeterLabel;
    Label outputMeterLabel;
    // -----
    Label titleLabel;
    Label nameLabel;

    //==============================================================================
    // This class manages the uniform values that the shaders use.
    struct Uniforms
    {
        Uniforms(OpenGLContext& openGLContext, OpenGLShaderProgram& shaderProgram);

        std::unique_ptr<OpenGLShaderProgram::Uniform>
            iResolution, iTime, iFrame, iChannel0, iChannel1, iChannel2, iLevel;

    private:
        static OpenGLShaderProgram::Uniform* createUniform(OpenGLContext& openGLContext,
                                                           OpenGLShaderProgram& shaderProgram,
                                                           const char* uniformName);
    };
    float iLevelAccum{};
    //==============================================================================
    OpenGLContext openGLContext;
    SpectrumDisplay spectrumDisplay;

    SliderWithMeter mixSlider;
    std::unique_ptr<SliderAttachment> mixAttachment;
    Label mixLabel;

    GLuint fboHandle;
    GLuint renderTex;
    GLuint depthBuf;

    Image componentMask;
    OpenGLTexture boundariesTexture;

    String vertexShader;
    String textureShader;
    String fragmentShader;

    std::unique_ptr<OpenGLShaderProgram> shaderProgram, bufferAProgram;
    std::unique_ptr<Shape> quad;
    std::unique_ptr<Attributes> attributes, attributes2;
    std::unique_ptr<Uniforms> uniforms, uniforms2;

    int frameCounter{};

    //==============================================================================
    typedef void (__stdcall *type_glDrawBuffers)(GLsizei n, const GLenum* bufs);

    type_glDrawBuffers glDrawBuffers;

    typedef void (__stdcall *type_glTexStorage2D)(GLenum target,
                                                  GLsizei levels,
                                                  GLenum internalformat,
                                                  GLsizei width,
                                                  GLsizei height);

    type_glTexStorage2D glTexStorage2D;
    //==============================================================================

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OvertoneFilterEditor)
};
