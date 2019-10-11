#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "MyMidiKeyboardComponent.h"
#include "LevelMeter.h"
#include "WavefrontObjParser.h"
#include "MySlider.h"
#include "SpectrumDisplay.h"

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

    MySlider mixSlider;
    std::unique_ptr<SliderAttachment> mixAttachment;
    Label mixLabel;

    // -----
    MySlider inputGainSlider;
    std::unique_ptr<SliderAttachment> inputGainAttachment;
    Label inputGainLabel;

    MySlider wetGainSlider;
    std::unique_ptr<SliderAttachment> wetGainAttachment;
    Label wetGainLabel;

    MySlider outputGainSlider;
    std::unique_ptr<SliderAttachment> outputGainAttachment;
    Label outputGainLabel;
    // -----

    LevelMeter inputMeter;
    LevelMeter wetMixMeter;
    LevelMeter outputMeter;

    Label inputMeterLabel;
    Label wetMixMeterLabel;
    Label outputMeterLabel;
    // -----
    DrawablePath bgPath;
    DrawablePath borderPath;
    Label nameLabel;

    // -------
    SpectrumDisplay spectrumDisplay;
    // -----
    OpenGLTexture boundariesTexture;

    //==============================================================================
    struct Vertex
    {
        float position[3];
        float normal[3];
        float colour[4];
        float texCoord[2];
    };

    //==============================================================================
    // This class just manages the attributes that the shaders use.
    struct Attributes
    {
        Attributes(OpenGLContext& openGLContext, OpenGLShaderProgram& shaderProgram);

        void enable(OpenGLContext& glContext);

        void disable(OpenGLContext& glContext);

        std::unique_ptr<OpenGLShaderProgram::Attribute> position;

    private:
        static OpenGLShaderProgram::Attribute* createAttribute(OpenGLContext& openGLContext,
                                                               OpenGLShaderProgram& shader,
                                                               const char* attributeName);
    };

    //==============================================================================
    // This class just manages the uniform values that the demo shaders use.
    struct Uniforms
    {
        Uniforms(OpenGLContext& openGLContext, OpenGLShaderProgram& shaderProgram);

        std::unique_ptr<OpenGLShaderProgram::Uniform>
            iResolution, iTime, slider0, iChannel0, iChannel1, iChannel2, iFrame, iSpectrum;

    private:
        static OpenGLShaderProgram::Uniform* createUniform(OpenGLContext& openGLContext,
                                                           OpenGLShaderProgram& shaderProgram,
                                                           const char* uniformName);
    };

    //==============================================================================
    /** This loads a 3D model from an OBJ file and converts it into some vertex buffers
        that we can draw.
    */
    struct Shape
    {
        Shape(OpenGLContext& glContext);

        void draw(OpenGLContext& glContext, Attributes& glAttributes);

    private:
        struct VertexBuffer
        {
            VertexBuffer(OpenGLContext& context, WavefrontObjFile::Shape& aShape);

            ~VertexBuffer();

            void bind();

            GLuint vertexBuffer, indexBuffer;
            int numIndices;
            OpenGLContext& openGLContext;

            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VertexBuffer)
        };

        WavefrontObjFile shapeFile;
        OwnedArray<VertexBuffer> vertexBuffers;

        static void createVertexListFromMesh(const WavefrontObjFile::Mesh& mesh, Array<Vertex>& list, Colour colour);
    };

    String vertexShader;
    String textureShader;
    String fragmentShader;

    std::unique_ptr<OpenGLShaderProgram> shaderProgram, bufferAProgram;
    std::unique_ptr<Shape> quad;
    std::unique_ptr<Attributes> attributes, attributes2;
    std::unique_ptr<Uniforms> uniforms, uniforms2;

    //==============================================================================

    /** The GL context */
    OpenGLContext openGLContext;
    int frameCounter{};

    typedef void (__stdcall *type_glDrawBuffers)(GLsizei n, const GLenum* bufs);

    type_glDrawBuffers glDrawBuffers;

    typedef void (__stdcall *type_glTexStorage2D)(GLenum target,
                                                  GLsizei levels,
                                                  GLenum internalformat,
                                                  GLsizei width,
                                                  GLsizei height);

    type_glTexStorage2D glTexStorage2D;

    GLuint fboHandle;
    GLuint renderTex;
    GLuint depthBuf;

    Image componentMask;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OvertoneFilterEditor)
};
