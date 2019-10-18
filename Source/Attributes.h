/*
  ==============================================================================

    Attributes.h
    Created: 14 Oct 2019 12:52:14pm
    Author:  Luke

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

// This class manages the attributes that the shaders use.
struct Attributes
{
    Attributes(OpenGLContext& openGLContext, OpenGLShaderProgram& shaderProgram);

    void enable(OpenGLContext& glContext) const;

    void disable(OpenGLContext& glContext) const;

    std::unique_ptr<OpenGLShaderProgram::Attribute> position;

private:
    static OpenGLShaderProgram::Attribute* createAttribute(OpenGLContext& openGLContext,
                                                           OpenGLShaderProgram& shader,
                                                           const char* attributeName);
};
