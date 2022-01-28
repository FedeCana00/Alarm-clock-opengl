#pragma once

#include <glad/glad.h>

class IGeometry
{
    public:
    virtual const GLfloat * vertices() = 0;               // location 0
    virtual const GLfloat * colors() { return NULL; }     // location 1
    virtual const GLfloat * normals() { return NULL; }    // location 2
    virtual const GLfloat * texCoords() { return NULL; }  // location 3
    virtual GLsizei verticesSize() = 0; // total number of vertices

    virtual const GLuint * faces() = 0; // faces: array of vertex indices for the EBO
    virtual GLsizei size() = 0;         // total number of vertex indices

    virtual GLenum type() = 0;          // type: GL_TRIANGLES, GL_LINES...

    virtual ~IGeometry() {}
};

class ModelRenderer
{
    public:
    explicit ModelRenderer(IGeometry & geo)
    {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        type = geo.type();
        size = geo.size();

        GLuint vertices_size = geo.verticesSize();

        const bool has_colors = geo.colors() != NULL;
        const bool has_normals = geo.normals() != NULL;
        const bool has_texCoords = geo.texCoords() != NULL;
        GLsizei stride = 3;
        if (has_colors)
            stride += 3;
        if (has_normals)
            stride += 3;
        if (has_texCoords)
            stride += 2;

        GLfloat * vertices = new GLfloat[vertices_size * stride];
        for (size_t i = 0; i < vertices_size; i++)
        {
            GLsizei offset = 0;
            vertices[i * stride + offset++] = geo.vertices()[i * 3 + 0];
            vertices[i * stride + offset++] = geo.vertices()[i * 3 + 1];
            vertices[i * stride + offset++] = geo.vertices()[i * 3 + 2];

            if (has_colors)
            {
                vertices[i * stride + offset++] = geo.colors()[i * 3 + 0];
                vertices[i * stride + offset++] = geo.colors()[i * 3 + 1];
                vertices[i * stride + offset++] = geo.colors()[i * 3 + 2];
            }

            if (has_normals)
            {
                vertices[i * stride + offset++] = geo.normals()[i * 3 + 0];
                vertices[i * stride + offset++] = geo.normals()[i * 3 + 1];
                vertices[i * stride + offset++] = geo.normals()[i * 3 + 2];
            }

            if (has_texCoords)
            {
                vertices[i * stride + offset++] = geo.texCoords()[i * 2 + 0];
                vertices[i * stride + offset++] = geo.texCoords()[i * 2 + 1];
            }
        }

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices_size * stride * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        delete[] vertices;

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(GLuint), geo.faces(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        GLsizei offset = 0;
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (void*)(offset * sizeof(GLfloat)));     // position: location 0
        glEnableVertexAttribArray(0);
        offset += 3;
        if (has_colors)
        {
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (void*)(offset * sizeof(GLfloat))); // color: location 1
            glEnableVertexAttribArray(1);
            offset += 3;
        }
        if (has_normals)
        {
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (void*)(offset * sizeof(GLfloat))); // normals: location 2
            glEnableVertexAttribArray(2);
            offset += 3;
        }
        if (has_texCoords)
        {
            glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (void*)(offset * sizeof(GLfloat))); // texCoords: location 3
            glEnableVertexAttribArray(3);
            offset += 2;
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

        glBindVertexArray(0);
    }

    ~ModelRenderer()
    {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
    }

    void render() const
    {
        renderRange(0, size);
    }

    void renderRange(GLsizei start, GLsizei count) const
    {
        glBindVertexArray(vao);
        glDrawElements(type, count, GL_UNSIGNED_INT, (void *)(start * sizeof(GLuint)));
        glBindVertexArray(0);
    }

    private:
    GLuint vao;
    GLuint vbo;
    GLuint ebo;

    GLuint size;
    GLenum type;
};

