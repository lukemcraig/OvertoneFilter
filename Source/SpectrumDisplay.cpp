/*
  ==============================================================================

    SpectrumDisplay.cpp
    Created: 9 Oct 2019 12:19:27am
    Author:  Luke

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "SpectrumDisplay.h"

//==============================================================================
SpectrumDisplay::
SpectrumDisplay(OvertoneFilterAudioProcessor& p, OpenGLContext& oc, SpectrumSource& iss, SpectrumSource& oss,
                ParameterHelper& parameterHelper) :
    openGLContext(oc),
    processor(p),
    inputSpectrumSource(iss), outputSpectrumSource(oss), parameterHelper(parameterHelper)
{
}

SpectrumDisplay::~SpectrumDisplay()
{
}

void SpectrumDisplay::paint(Graphics& g)
{
}

void SpectrumDisplay::resized()
{
}

void SpectrumDisplay::initialiseOpenGL()
{
    createShaders();
}

void SpectrumDisplay::shutdown()
{
    shaderProgram.reset();

    quad.reset();
    attributes.reset();
    uniforms.reset();

    spectrumTexture.release();
}

void SpectrumDisplay::renderScene()
{
    //render scene
    auto desktopScale = (float)openGLContext.getRenderingScale();
    auto width = roundToInt(desktopScale * getWidth());
    auto height = roundToInt(desktopScale * getHeight());

    auto x = getRight() * desktopScale - width;
    auto y = (getParentHeight() - getBottom()) * desktopScale;
    glViewport(x, y, width, height);

    shaderProgram->use();

    if (uniforms->iResolution != nullptr)
    {
        uniforms->iResolution->set(width, height);
    }

    if (uniforms->iViewport != nullptr)
    {
        uniforms->iViewport->set(x, y);
    }

    if (uniforms->iSpectrum != nullptr)
    {
        bool needToUpdate = false;
        if (inputSpectrumSource.getSpectrum(spectrumImage, 0))
        {
            needToUpdate = true;
        }
        if (outputSpectrumSource.getSpectrum(spectrumImage, 1))
        {
            needToUpdate = true;
        }
        if (needToUpdate)
        {
            spectrumTexture.loadImage(spectrumImage);
            glBindTexture(GL_TEXTURE_2D, spectrumTexture.getTextureID());

            jassert(spectrumTexture.getTextureID()==3);
            uniforms->iSpectrum->set(3);
        }
    }

    if (uniforms->iPitchStandard != nullptr)
    {
        uniforms->iPitchStandard->set(parameterHelper.getCurrentPitchStandard(0));
    }

    quad->draw(openGLContext, *attributes);
}

void SpectrumDisplay::render()
{
    jassert(OpenGLHelpers::isContextActive());
    //OpenGLHelpers::clear(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    {
        openGLContext.extensions.glActiveTexture(GL_TEXTURE2 + 1);
        spectrumTexture.bind();
    }

    renderScene();

    // Reset the element buffers so child Components draw correctly
    openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, 0);
    openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void SpectrumDisplay::createShaders()
{
    vertexShader =
        "attribute vec4 position;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(position.xy*5.0,0.0,1.0);\n"
        "}\n";

    // todo sample rate and min and max notes
    fragmentShader =
        "#define minNote 0.0\n"
        "#define maxNote 127.0\n"
        "uniform vec2 iResolution;\n"
        "uniform vec2 iViewport;\n"
        "uniform float iPitchStandard;\n"
        "uniform sampler2D iSpectrum;\n"

        "void main()\n"
        "{\n"
        "    // Normalized pixel coordinates (from 0 to 1)\n"
        "    vec2 uv = (gl_FragCoord.xy-iViewport.xy)/iResolution.xy;\n"
        "    float x = uv.x;\n"
        "    x = (iPitchStandard * pow(2.0,(x * (maxNote-minNote)+minNote - 69.0)/12.0))/22050.0;\n"
        "    float fftinput = texture(iSpectrum,vec2(x,1.0)).r;\n"
        "    float fftoutput = texture(iSpectrum,vec2(x,0.0)).r;\n"
        "    if(fftoutput>uv.y){\n"
        "    gl_FragColor = vec4(0.0,0.0,0.0,1.0);\n"
        "    }\n"
        "    else if(fftinput>uv.y){\n"
        "    gl_FragColor = vec4(0.2,0.2,0.2,1.0);\n"
        "    }\n"
        "    else{\n"
        "    gl_FragColor = vec4(0.0,0.0,0.0,0.0);\n"
        "    }\n"
        "    \n"
        //"    float maskin = clamp(sign(fftinput - uv.y),0.0,1.0);\n"
        //"    float maskout = clamp(sign(fftoutput - uv.y),0.0,1.0);\n"
        //"    vec4 colin = vec4(vec3(0.0,1.0,0.2),maskin);\n"
        //"    vec4 colout = vec4(vec3(1.0,0.4,0.2),maskout);\n"
        //"    gl_FragColor = colout+colin;\n"
        "}\n";

    quad.reset(new Shape(openGLContext));

    std::unique_ptr<OpenGLShaderProgram> newShader(new OpenGLShaderProgram(openGLContext));

    if (newShader->addVertexShader(OpenGLHelpers::translateVertexShaderToV3(vertexShader))
        && newShader->addFragmentShader("#version 410\n"
            "out " JUCE_MEDIUMP " vec4 fragColor;\n"
            + String(fragmentShader).replace("varying", "in")
                                    .replace("texture2D", "texture")
                                    .replace("gl_FragColor", "fragColor"))
        && newShader->link())
    {
        attributes.reset();
        uniforms.reset();

        shaderProgram.reset(newShader.release());
        shaderProgram->use();

        attributes.reset(new Attributes(openGLContext, *shaderProgram));
        uniforms.reset(new Uniforms(openGLContext, *shaderProgram));
    }
    else
    {
        DBG(newShader->getLastError());
    }
}

void SpectrumDisplay::renderOpenGL()
{
    render();
}

//==============================================================================

SpectrumDisplay::Uniforms::Uniforms(OpenGLContext& openGLContext, OpenGLShaderProgram& shaderProgram)
{
    iResolution.reset(createUniform(openGLContext, shaderProgram, "iResolution"));
    iSpectrum.reset(createUniform(openGLContext, shaderProgram, "iSpectrum"));
    iViewport.reset(createUniform(openGLContext, shaderProgram, "iViewport"));
    iPitchStandard.reset(createUniform(openGLContext, shaderProgram, "iPitchStandard"));
}

OpenGLShaderProgram::Uniform* SpectrumDisplay::Uniforms::createUniform(OpenGLContext& openGLContext,
                                                                       OpenGLShaderProgram&
                                                                       shaderProgram,
                                                                       const char* uniformName)
{
    if (openGLContext.extensions.glGetUniformLocation(shaderProgram.getProgramID(), uniformName) < 0)
        return nullptr;

    return new OpenGLShaderProgram::Uniform(shaderProgram, uniformName);
}
