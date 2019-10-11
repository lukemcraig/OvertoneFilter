/*
  ==============================================================================

    SpectrumDisplay.h
    Created: 9 Oct 2019 12:19:27am
    Author:  Luke

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "WavefrontObjParser.h"
#include "PluginProcessor.h"

//==============================================================================
/*
*/
class SpectrumDisplay : public Component
{
public:
    SpectrumDisplay(OvertoneFilterAudioProcessor&, OpenGLContext&, SpectrumSource&);

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
    SpectrumSource& spectrumSource;
    OpenGLTexture spectrumTexture;
    Image spectrumImage = Image(Image::PixelFormat::ARGB, SpectrumSource::fftSizePositive, 1, true);

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
            iResolution, iTime, slider0, iChannel0, iChannel1, iFrame, iSpectrum, iViewport;

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

    std::unique_ptr<OpenGLShaderProgram> shaderProgram;
    std::unique_ptr<Shape> quad;
    std::unique_ptr<Attributes> attributes;
    std::unique_ptr<Uniforms> uniforms;

    //==============================================================================

    /** The GL context */
    OpenGLContext& openGLContext;
    int frameCounter{};

    GLuint fboHandle;
    GLuint renderTex;
    GLuint depthBuf;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrumDisplay)
};
