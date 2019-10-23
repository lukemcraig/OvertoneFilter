/*
  ==============================================================================

    Shape.cpp
    Created: 14 Oct 2019 12:52:30pm
    Author:  Luke

  ==============================================================================
*/

#include "Shape.h"

Shape::Shape(OpenGLContext& glContext)
{
    const String objFileContent{
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

void Shape::draw(OpenGLContext& glContext, Attributes& glAttributes)
{
    for (auto* vertexBuffer : vertexBuffers)
    {
        vertexBuffer->bind();

        glAttributes.enable(glContext);
        glDrawElements(GL_TRIANGLES, vertexBuffer->numIndices, GL_UNSIGNED_INT, nullptr);
        glAttributes.disable(glContext);
    }
}

Shape::VertexBuffer::VertexBuffer(OpenGLContext& context,
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
                                              uint32)),
                                          aShape.mesh.indices.getRawDataPointer(), GL_STATIC_DRAW);
}

Shape::VertexBuffer::~VertexBuffer()
{
    openGLContext.extensions.glDeleteBuffers(1, &vertexBuffer);
    openGLContext.extensions.glDeleteBuffers(1, &indexBuffer);
}

void Shape::VertexBuffer::bind() const
{
    openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
}

void Shape::createVertexListFromMesh(const WavefrontObjFile::Mesh& mesh,
                                     Array<Vertex>& list, const Colour colour)
{
    const auto scale = 0.2f;
    const WavefrontObjFile::TextureCoord defaultTexCoord{0.5f, 0.5f};
    const WavefrontObjFile::Vertex defaultNormal{0.5f, 0.5f, 0.5f};

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
