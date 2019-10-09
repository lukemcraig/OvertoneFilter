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
SpectrumDisplay::SpectrumDisplay(OpenGLContext& openGlContext) : openGLContext(openGlContext)
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

    if (uniforms->iTime != nullptr)
    {
        const float sec = Time::getMillisecondCounterHiRes() * 0.001f;
        uniforms->iTime->set(sec);
    }

    if (uniforms->iFrame != nullptr)
    {
        uniforms->iFrame->set(frameCounter);
    }

    quad->draw(openGLContext, *attributes);
}

void SpectrumDisplay::render()
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
        "uniform int iFrame;\n"
        "uniform float iTime;\n"
        "uniform vec2 iResolution;\n"
        "uniform vec2 iViewport;\n"
        "uniform float slider0;\n"

        "#define quietColor vec3(0.0, 1.0, 0.0)\n"
        "#define loudColor vec3(1.0, 0.0, 0.0)\n"

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

void SpectrumDisplay::renderOpenGL()
{
    render();
    ++frameCounter;
}

//==============================================================================
//==============================================================================

SpectrumDisplay::Attributes::Attributes(OpenGLContext& openGLContext, OpenGLShaderProgram& shaderProgram)
{
    position.reset(createAttribute(openGLContext, shaderProgram, "position"));
}

void SpectrumDisplay::Attributes::enable(OpenGLContext& glContext)
{
    if (position.get() != nullptr)
    {
        glContext.extensions.glVertexAttribPointer(position->attributeID, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                                   nullptr);
        glContext.extensions.glEnableVertexAttribArray(position->attributeID);
    }
}

void SpectrumDisplay::Attributes::disable(OpenGLContext& glContext)
{
    if (position.get() != nullptr) glContext.extensions.glDisableVertexAttribArray(position->attributeID);
}

OpenGLShaderProgram::Attribute* SpectrumDisplay::Attributes::createAttribute(
    OpenGLContext& openGLContext, OpenGLShaderProgram& shader, const char* attributeName)
{
    if (openGLContext.extensions.glGetAttribLocation(shader.getProgramID(), attributeName) < 0)
        return nullptr;

    return new OpenGLShaderProgram::Attribute(shader, attributeName);
}

SpectrumDisplay::Uniforms::Uniforms(OpenGLContext& openGLContext, OpenGLShaderProgram& shaderProgram)
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

OpenGLShaderProgram::Uniform* SpectrumDisplay::Uniforms::createUniform(OpenGLContext& openGLContext,
                                                                  OpenGLShaderProgram&
                                                                  shaderProgram,
                                                                  const char* uniformName)
{
    if (openGLContext.extensions.glGetUniformLocation(shaderProgram.getProgramID(), uniformName) < 0)
        return nullptr;

    return new OpenGLShaderProgram::Uniform(shaderProgram, uniformName);
}

SpectrumDisplay::Shape::Shape(OpenGLContext& glContext)
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

void SpectrumDisplay::Shape::draw(OpenGLContext& glContext, Attributes& glAttributes)
{
    for (auto* vertexBuffer : vertexBuffers)
    {
        vertexBuffer->bind();

        glAttributes.enable(glContext);
        glDrawElements(GL_TRIANGLES, vertexBuffer->numIndices, GL_UNSIGNED_INT, nullptr);
        glAttributes.disable(glContext);
    }
}

SpectrumDisplay::Shape::VertexBuffer::VertexBuffer(OpenGLContext& context,
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

SpectrumDisplay::Shape::VertexBuffer::~VertexBuffer()
{
    openGLContext.extensions.glDeleteBuffers(1, &vertexBuffer);
    openGLContext.extensions.glDeleteBuffers(1, &indexBuffer);
}

void SpectrumDisplay::Shape::VertexBuffer::bind()
{
    openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
}

void SpectrumDisplay::Shape::createVertexListFromMesh(const WavefrontObjFile::Mesh& mesh,
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
