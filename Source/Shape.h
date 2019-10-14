/*
  ==============================================================================

    Shape.h
    Created: 14 Oct 2019 12:52:30pm
    Author:  Luke

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "Attributes.h"
#include "WavefrontObjParser.h"
#include "Vertex.h"

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
