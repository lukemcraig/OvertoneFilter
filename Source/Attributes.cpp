/*
  ==============================================================================

    Attributes.cpp
    Created: 14 Oct 2019 12:52:14pm
    Author:  Luke

  ==============================================================================
*/

#include "Attributes.h"
#include "Vertex.h"

Attributes::Attributes(OpenGLContext& openGLContext, OpenGLShaderProgram& shaderProgram)
{
    position.reset(createAttribute(openGLContext, shaderProgram, "position"));
}

void Attributes::enable(OpenGLContext& glContext)
{
    if (position.get() != nullptr)
    {
        glContext.extensions.glVertexAttribPointer(position->attributeID, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                                   nullptr);
        glContext.extensions.glEnableVertexAttribArray(position->attributeID);
    }
}

void Attributes::disable(OpenGLContext& glContext)
{
    if (position.get() != nullptr) glContext.extensions.glDisableVertexAttribArray(position->attributeID);
}

OpenGLShaderProgram::Attribute* Attributes::createAttribute(
    OpenGLContext& openGLContext, OpenGLShaderProgram& shader, const char* attributeName)
{
    if (openGLContext.extensions.glGetAttribLocation(shader.getProgramID(), attributeName) < 0)
        return nullptr;

    return new OpenGLShaderProgram::Attribute(shader, attributeName);
}
