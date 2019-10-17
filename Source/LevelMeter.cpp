/*
  ==============================================================================

    LevelMeter.cpp
    Created: 2 Oct 2019 3:52:35pm
    Author:  Luke

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "LevelMeter.h"

//==============================================================================
LevelMeter::LevelMeter(LevelMeterAudioSource& lmas, OpenGLContext& oglc) :
    levelMeterAudioSource(lmas), openGLContext(oglc)
{
    setOpaque(true);
}

LevelMeter::~LevelMeter()
{
}

void LevelMeter::paint(Graphics& g)
{
    auto lineCol = getLookAndFeel().findColour(MidiKeyboardComponent::keySeparatorLineColourId);
    auto area = getLocalBounds();
    g.setColour(lineCol);    
    g.drawRect(area);
    area.reduce(2, 2);
    g.setColour(Colour(0xffB28859));
    g.drawRect(area);
    area.reduce(2, 2);
    g.setColour(lineCol);  
    g.drawRect(area);
    area.reduce(2, 2);
    g.setColour(Colour(0xffB28859));
    g.drawRect(area);
    area.reduce(2, 2);
}

void LevelMeter::resized()
{
}

void LevelMeter::initialiseOpenGL()
{
    createShaders();
}

void LevelMeter::shutdown()
{
    shaderProgram.reset();

    quad.reset();
    attributes.reset();
    uniforms.reset();
}

void LevelMeter::renderScene()
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

    if (uniforms->iLevel != nullptr)
    {
        //todo
        const auto rms = 1.4125375446227544f * levelMeterAudioSource.getLevel();
        uniforms->iLevel->set(rms);
    }

    quad->draw(openGLContext, *attributes);
}

void LevelMeter::render()
{
    jassert(OpenGLHelpers::isContextActive());

    renderScene();

    // Reset the element buffers so child Components draw correctly
    openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, 0);
    openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void LevelMeter::createShaders()
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
        "uniform float iLevel;\n"

        "#define purp vec3(.208, 0.196, 0.475)\n"
        "#define green vec3(.518, .698, .353)\n"
        "#define gold vec3( .698, .533, .349)\n"

        "void main()\n"
        "{\n"
        "    // Normalized pixel coordinates (from 0 to 1)\n"
        "    vec2 uv = (gl_FragCoord.xy-iViewport.xy)/iResolution.xy;\n"
        //"    vec3 col = mix(quietColor, loudColor, uv.x);\n"
        // "    vec3 col = gold;\n"
        "    float mask = clamp(sign(iLevel - uv.x),0.0,1.0);\n"
        "    gl_FragColor = vec4(mix(purp,gold,mask), 1.0);\n"
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

void LevelMeter::renderOpenGL()
{
    render();
}

//==============================================================================

LevelMeter::Uniforms::Uniforms(OpenGLContext& openGLContext, OpenGLShaderProgram& shaderProgram)
{
    iResolution.reset(createUniform(openGLContext, shaderProgram, "iResolution"));
    iViewport.reset(createUniform(openGLContext, shaderProgram, "iViewport"));
    iLevel.reset(createUniform(openGLContext, shaderProgram, "iLevel"));
}

OpenGLShaderProgram::Uniform* LevelMeter::Uniforms::createUniform(OpenGLContext& openGLContext,
                                                                  OpenGLShaderProgram&
                                                                  shaderProgram,
                                                                  const char* uniformName)
{
    if (openGLContext.extensions.glGetUniformLocation(shaderProgram.getProgramID(), uniformName) < 0)
        return nullptr;

    return new OpenGLShaderProgram::Uniform(shaderProgram, uniformName);
}
