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
LevelMeter::LevelMeter(LevelMeterAudioSource& lmas, OpenGLContext& oglc, Colour c, int numLEDsm) :
    levelMeterAudioSource(lmas), openGLContext(oglc),
    clipColour(c),
    numLEDs(numLEDs)
{
    setOpaque(true);
}

LevelMeter::~LevelMeter()
{
}

void LevelMeter::paint(Graphics& g)
{
    if (true)
    {
        //g.fillAll(Colours::blue);
        return;
    }
    //adapted from LookAndFeel_V4::drawLevelMeter()
    //getLookAndFeel().drawLevelMeter(g,getWidth(),getHeight(),1.0f);

    const auto width = getWidth();
    const auto height = getHeight();

    const auto outerCornerSize = 3.0f;
    const auto outerBorderWidth = 2.0f;
    const auto spacingFraction = 0.03f;

    g.setColour(findColour(ResizableWindow::backgroundColourId));
    g.fillRoundedRectangle(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), outerCornerSize);

    const auto doubleOuterBorderWidth = 2.0f * outerBorderWidth;

    auto c = findColour(Slider::thumbColourId);

    const auto rms = 1.4125375446227544f * levelMeterAudioSource.getLevel();
    const auto db = Decibels::gainToDecibels(rms);
    const auto noiseFloor = -60.0f;
    const auto a = (numLEDs - 1.0f) / (noiseFloor * noiseFloor);
    const auto dbGated = std::max(0.0f, db - noiseFloor);

    const auto numBlocks = static_cast<int>(std::ceil(a * dbGated * dbGated));
    if (height > width)
    {
        const auto blockWidth = width - doubleOuterBorderWidth;
        const auto blockHeight = (height - doubleOuterBorderWidth) / static_cast<float>(numLEDs);
        const auto blockRectHeight = (1.0f - 2.0f * spacingFraction) * blockHeight;
        const auto blockRectSpacing = spacingFraction * blockHeight;

        const auto blockCornerSize = 0.1f * blockWidth;

        for (auto i = 0; i < numLEDs; ++i)
        {
            if (i >= numBlocks)
                g.setColour(c.withAlpha(0.5f));
            else
                g.setColour(i < numLEDs - 1 ? c : clipColour);

            g.fillRoundedRectangle(outerBorderWidth,
                                   outerBorderWidth + ((numLEDs - i - 1) * blockHeight) + blockRectSpacing,
                                   blockWidth,
                                   blockRectHeight,
                                   blockCornerSize);
        }
    }
    else
    {
        const auto blockWidth = (width - doubleOuterBorderWidth) / static_cast<float>(numLEDs);
        const auto blockHeight = height - doubleOuterBorderWidth;

        const auto blockRectWidth = (1.0f - 2.0f * spacingFraction) * blockWidth;
        const auto blockRectSpacing = spacingFraction * blockWidth;

        const auto blockCornerSize = 0.1f * blockWidth;

        for (auto i = 0; i < numLEDs; ++i)
        {
            if (i >= numBlocks)
                g.setColour(c.withAlpha(0.5f));
            else
                g.setColour(i < numLEDs - 1 ? c : Colours::red);

            g.fillRoundedRectangle(outerBorderWidth + (i * blockWidth) + blockRectSpacing,
                                   outerBorderWidth,
                                   blockRectWidth,
                                   blockHeight,
                                   blockCornerSize);
        }
    }
}

void LevelMeter::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
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

    auto x = getRight()*desktopScale - width;
    auto y = (getParentHeight() - getBottom())*desktopScale;
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

    if (uniforms->iTime != nullptr)
    {
        const float sec = Time::getMillisecondCounterHiRes() * 0.001f;
        uniforms->iTime->set(sec);
    }

    if (uniforms->iFrame != nullptr)
    {
        uniforms->iFrame->set(frameCounter);
    }

    if (uniforms->slider0 != nullptr)
    {
        //todo
        const auto rms = 1.4125375446227544f * levelMeterAudioSource.getLevel();
        uniforms->slider0->set(rms);
    }

    //if (uniforms->iChannel0 != nullptr)
    //{
    //    // Use the texture that is associated with the FBO
    //    uniforms->iChannel0->set(0);
    //}

    quad->draw(openGLContext, *attributes);
}

void LevelMeter::render()
{
    jassert(OpenGLHelpers::isContextActive());
    //OpenGLHelpers::clear(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
    //glEnable(GL_BLEND | GL_DEPTH);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
        "uniform int iFrame;\n"
        "uniform float iTime;\n"
        "uniform vec2 iResolution;\n"
        "uniform vec2 iViewport;\n"
        "uniform float slider0;\n"

        "#define quietColor vec3(0.0, 0.0, 0.0)\n"
        "#define loudColor vec3(1.0, 1.0, 1.0)\n"

        "void main()\n"
        "{\n"
        "    // Normalized pixel coordinates (from 0 to 1)\n"
        "    vec2 uv = (gl_FragCoord.xy-iViewport.xy)/iResolution.xy;\n"
        "    float level  = slider0;\n"
        "    vec3 col = mix(quietColor, loudColor, uv.x);\n"
        "    float mask = clamp(sign(level - uv.x),0.3,1.0);\n"
        "    gl_FragColor = vec4(col*mask, 1.0);\n"
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
    ++frameCounter;
}


//==============================================================================

LevelMeter::Attributes::Attributes(OpenGLContext& openGLContext, OpenGLShaderProgram& shaderProgram)
{
    position.reset(createAttribute(openGLContext, shaderProgram, "position"));
}

void LevelMeter::Attributes::enable(OpenGLContext& glContext)
{
    if (position.get() != nullptr)
    {
        glContext.extensions.glVertexAttribPointer(position->attributeID, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                                   nullptr);
        glContext.extensions.glEnableVertexAttribArray(position->attributeID);
    }
}

void LevelMeter::Attributes::disable(OpenGLContext& glContext)
{
    if (position.get() != nullptr) glContext.extensions.glDisableVertexAttribArray(position->attributeID);
}

OpenGLShaderProgram::Attribute* LevelMeter::Attributes::createAttribute(
    OpenGLContext& openGLContext, OpenGLShaderProgram& shader, const char* attributeName)
{
    if (openGLContext.extensions.glGetAttribLocation(shader.getProgramID(), attributeName) < 0)
        return nullptr;

    return new OpenGLShaderProgram::Attribute(shader, attributeName);
}

LevelMeter::Uniforms::Uniforms(OpenGLContext& openGLContext, OpenGLShaderProgram& shaderProgram)
{
    iResolution.reset(createUniform(openGLContext, shaderProgram, "iResolution"));
    iTime.reset(createUniform(openGLContext, shaderProgram, "iTime"));
    iFrame.reset(createUniform(openGLContext, shaderProgram, "iFrame"));
    slider0.reset(createUniform(openGLContext, shaderProgram, "slider0"));
    iChannel0.reset(createUniform(openGLContext, shaderProgram, "iChannel0"));
    iChannel1.reset(createUniform(openGLContext, shaderProgram, "iChannel1"));
    iSpectrum.reset(createUniform(openGLContext, shaderProgram, "iSpectrum"));
    iViewport.reset(createUniform(openGLContext, shaderProgram, "iViewport"));
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

LevelMeter::Shape::Shape(OpenGLContext& glContext)
{
    //auto objFileContent = loadEntireAssetIntoString("quad.obj");
    String objFileContent{
        "v -1.000000 -1.000000 0.000000\n"
        "v 1.000000 -1.000000 0.000000\n"
        "v -1.000000 1.000000 -0.000000\n"
        "v 1.000000 1.000000 -0.000000\n"
        "f 2 3 1\n"
        "f 2 4 3\n"
    };
    if (shapeFile.load(objFileContent).wasOk())
        for (auto* shapeVertices : shapeFile.shapes)
            vertexBuffers.add(new VertexBuffer(glContext, *shapeVertices));
}

void LevelMeter::Shape::draw(OpenGLContext& glContext, Attributes& glAttributes)
{
    for (auto* vertexBuffer : vertexBuffers)
    {
        vertexBuffer->bind();

        glAttributes.enable(glContext);
        glDrawElements(GL_TRIANGLES, vertexBuffer->numIndices, GL_UNSIGNED_INT, nullptr);
        glAttributes.disable(glContext);
    }
}

LevelMeter::Shape::VertexBuffer::VertexBuffer(OpenGLContext& context,
                                              WavefrontObjFile::Shape& aShape): openGLContext(
    context)
{
    numIndices = aShape.mesh.indices.size();

    openGLContext.extensions.glGenBuffers(1, &vertexBuffer);
    openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

    Array<Vertex> vertices;
    createVertexListFromMesh(aShape.mesh, vertices, Colours::green);

    openGLContext.extensions.glBufferData(GL_ARRAY_BUFFER,
                                          static_cast<GLsizeiptr>(static_cast<size_t>(vertices.size()) *
                                              sizeof(Vertex)),
                                          vertices.getRawDataPointer(), GL_STATIC_DRAW);

    openGLContext.extensions.glGenBuffers(1, &indexBuffer);
    openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    openGLContext.extensions.glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                                          static_cast<GLsizeiptr>(static_cast<size_t>(numIndices) * sizeof(
                                              juce::uint32)),
                                          aShape.mesh.indices.getRawDataPointer(), GL_STATIC_DRAW);
}

LevelMeter::Shape::VertexBuffer::~VertexBuffer()
{
    openGLContext.extensions.glDeleteBuffers(1, &vertexBuffer);
    openGLContext.extensions.glDeleteBuffers(1, &indexBuffer);
}

void LevelMeter::Shape::VertexBuffer::bind()
{
    openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
}

void LevelMeter::Shape::createVertexListFromMesh(const WavefrontObjFile::Mesh& mesh,
                                                 Array<Vertex>& list, Colour colour)
{
    auto scale = 0.2f;
    WavefrontObjFile::TextureCoord defaultTexCoord{0.5f, 0.5f};
    WavefrontObjFile::Vertex defaultNormal{0.5f, 0.5f, 0.5f};

    for (auto i = 0; i < mesh.vertices.size(); ++i)
    {
        const auto& v = mesh.vertices.getReference(i);
        const auto& n = i < mesh.normals.size() ? mesh.normals.getReference(i) : defaultNormal;
        const auto& tc = i < mesh.textureCoords.size() ? mesh.textureCoords.getReference(i) : defaultTexCoord;

        list.add({
            {scale * v.x, scale * v.y, scale * v.z,},
            {scale * n.x, scale * n.y, scale * n.z,},
            {colour.getFloatRed(), colour.getFloatGreen(), colour.getFloatBlue(), colour.getFloatAlpha()},
            {tc.x, tc.y}
        });
    }
}
