/*
  ==============================================================================

    SpectrumDisplay.cpp
    Created: 9 Oct 2019 12:19:27am
    Author:  Luke McDuffie Craig

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "SpectrumDisplay.h"

//==============================================================================
SpectrumDisplay::
SpectrumDisplay(OvertoneFilterAudioProcessor& p, OpenGLContext& oc, SpectrumSource& iss, SpectrumSource& oss,
                ParameterHelper& parameterHelper) :
    processor(p),
    openGLContext(oc),
    parameterHelper(parameterHelper),
    inputSpectrumSource(iss),
    outputSpectrumSource(oss)
{
}

SpectrumDisplay::~SpectrumDisplay()
{
}

void SpectrumDisplay::paint(Graphics& /*g*/)
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
    const auto desktopScale = static_cast<float>(openGLContext.getRenderingScale());
    const auto width = roundToInt(desktopScale * static_cast<float>(getWidth()));
    const auto height = roundToInt(desktopScale * static_cast<float>(getHeight()));

    const auto x = static_cast<float>(getRight()) * desktopScale - width;
    const auto y = static_cast<float>(getParentHeight() - getBottom()) * desktopScale;

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
        auto needToUpdate = false;
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
            const auto spectrumTextureId = spectrumTexture.getTextureID();
            glBindTexture(GL_TEXTURE_2D, spectrumTextureId);

            jassert(spectrumTextureId==3);

            uniforms->iSpectrum->set(static_cast<GLint>(spectrumTextureId));
        }
    }

    if (uniforms->iPitchStandard != nullptr)
    {
        uniforms->iPitchStandard->set(parameterHelper.getCurrentPitchStandard(0));
    }

    if (uniforms->iMinNote != nullptr)
    {
        uniforms->iMinNote->set(0.0f);
    }

    if (uniforms->iMaxNote != nullptr)
    {
        uniforms->iMaxNote->set(127.0f);
    }

    if (uniforms->iNyquist != nullptr)
    {
        const auto nyq = inputSpectrumSource.getSampleRate() / 2.0;
        uniforms->iNyquist->set(static_cast<GLfloat>(nyq));
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

    fragmentShader =
        "uniform vec2 iResolution;\n"
        "uniform vec2 iViewport;\n"
        "uniform float iPitchStandard;\n"
        "uniform sampler2D iSpectrum;\n"
        "uniform float iMinNote;\n"
        "uniform float iMaxNote;\n"
        "uniform float iNyquist;\n"

        "void main()\n"
        "{\n"
        "    // Normalized pixel coordinates (from 0 to 1)\n"
        "    vec2 uv = (gl_FragCoord.xy-iViewport.xy)/iResolution.xy;\n"
        "    float x = uv.x;\n"
        "    x = (iPitchStandard * pow(2.0,(x * (iMaxNote-iMinNote)+iMinNote - 69.0)/12.0))/iNyquist;\n"
        "    float fftinput = texture(iSpectrum,vec2(x,1.0)).r;\n"
        "    float fftoutput = texture(iSpectrum,vec2(x,0.0)).r;\n"
        "    vec3 specCol = vec3(  .208, 0.196, 0.475);\n"
        "    vec3 lineCol = vec3( .0, .0, .0);\n"
        "    if(uv.y<=fftoutput){\n"
        "       if(uv.y>=fftoutput-0.02){\n"
        "           gl_FragColor = vec4( .0, .0, .0,0.5);\n"
        "       }\n"
        "       else{\n"
        "           gl_FragColor = vec4(mix(specCol,lineCol,mod(distance(uv.y,fftoutput),0.10 ) ),1.0);\n"
        "       }\n"
        "    }\n"
        "    else if(fftinput>uv.y){\n"
        "       gl_FragColor = vec4(0.9,0.9,0.9,0.9);\n"
        "    }\n"
        "    else{\n"
        "       gl_FragColor = vec4(0.0,0.0,0.0,0.0);\n"
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
    iMinNote.reset(createUniform(openGLContext, shaderProgram, "iMinNote"));
    iMaxNote.reset(createUniform(openGLContext, shaderProgram, "iMaxNote"));
    iNyquist.reset(createUniform(openGLContext, shaderProgram, "iNyquist"));
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
