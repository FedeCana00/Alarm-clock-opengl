#ifndef CYLINDER_GEOMETRY_H
#define CYLINDER_GEOMETRY_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "model_renderer.h"
#include "compute_normals.h"

class CylinderGeometry : public IGeometry
{
public:
    CylinderGeometry(float radius, float height, glm::vec3 color_top, glm::vec3 color_bottom, glm::vec3 color_side)
    {
        const int SAMPLES = 30;
        m_vertices = new GLfloat[3 * (4 * SAMPLES + 2)];
        m_colors = new GLfloat[3 * (4 * SAMPLES + 2)];
        m_normals = new GLfloat[3 * (4 * SAMPLES + 2)];
        m_faces = new GLuint[3 * (2 * SAMPLES + SAMPLES + SAMPLES)];

        int face_counter = 0;
        int vertex_counter = 0;

        int zero_vertex;

        // top face (circle)
        zero_vertex = vertex_counter; // save initial vertex index

        m_vertices[vertex_counter * 3 + 0] = 0.0f;
        m_vertices[vertex_counter * 3 + 1] = 0.0f;
        m_vertices[vertex_counter * 3 + 2] = height / 2.0f;

        m_colors[vertex_counter * 3 + 0] = color_top.r;
        m_colors[vertex_counter * 3 + 1] = color_top.g;
        m_colors[vertex_counter * 3 + 2] = color_top.b;

        m_normals[vertex_counter * 3 + 0] = 0.0f;
        m_normals[vertex_counter * 3 + 1] = 0.0f;
        m_normals[vertex_counter * 3 + 2] = 1.0f;
        vertex_counter++;

        for (int i = 0; i < SAMPLES; i++)
        {
            const float angle = float(i) / float(SAMPLES) * glm::pi<float>() * 2.0f;
            const float x = radius * std::cos(angle);
            const float y = radius * std::sin(angle);
            const float z = height / 2.0f;

            m_vertices[vertex_counter * 3 + 0] = x;
            m_vertices[vertex_counter * 3 + 1] = y;
            m_vertices[vertex_counter * 3 + 2] = z;

            m_colors[vertex_counter * 3 + 0] = color_top.r;
            m_colors[vertex_counter * 3 + 1] = color_top.g;
            m_colors[vertex_counter * 3 + 2] = color_top.b;

            m_normals[vertex_counter * 3 + 0] = 0.0f;
            m_normals[vertex_counter * 3 + 1] = 0.0f;
            m_normals[vertex_counter * 3 + 2] = 1.0f;
            vertex_counter++;
        }

        for (int i = 0; i < SAMPLES; i++)
        {
            m_faces[face_counter * 3 + 0] = zero_vertex;
            m_faces[face_counter * 3 + 1] = zero_vertex + 1 + i;
            m_faces[face_counter * 3 + 2] = zero_vertex + 1 + (i + 1) % SAMPLES;
            face_counter++;
        }

        // bottom face (circle)
        zero_vertex = vertex_counter; // save initial vertex index

        m_vertices[vertex_counter * 3 + 0] = 0.0f;
        m_vertices[vertex_counter * 3 + 1] = 0.0f;
        m_vertices[vertex_counter * 3 + 2] = -height / 2.0f;

        m_colors[vertex_counter * 3 + 0] = color_bottom.r;
        m_colors[vertex_counter * 3 + 1] = color_bottom.g;
        m_colors[vertex_counter * 3 + 2] = color_bottom.b;

        m_normals[vertex_counter * 3 + 0] = 0.0f;
        m_normals[vertex_counter * 3 + 1] = 0.0f;
        m_normals[vertex_counter * 3 + 2] = -1.0f;
        vertex_counter++;

        for (int i = 0; i < SAMPLES; i++)
        {
            const float angle = float(i) / float(SAMPLES) * glm::pi<float>() * 2.0f;
            const float x = radius * std::cos(angle);
            const float y = radius * std::sin(angle);
            const float z = -height / 2.0f;

            m_vertices[vertex_counter * 3 + 0] = x;
            m_vertices[vertex_counter * 3 + 1] = y;
            m_vertices[vertex_counter * 3 + 2] = z;

            m_colors[vertex_counter * 3 + 0] = color_bottom.r;
            m_colors[vertex_counter * 3 + 1] = color_bottom.g;
            m_colors[vertex_counter * 3 + 2] = color_bottom.b;

            m_normals[vertex_counter * 3 + 0] = 0.0f;
            m_normals[vertex_counter * 3 + 1] = 0.0f;
            m_normals[vertex_counter * 3 + 2] = -1.0f;
            vertex_counter++;
        }

        for (int i = 0; i < SAMPLES; i++)
        {
            m_faces[face_counter * 3 + 0] = zero_vertex;
            m_faces[face_counter * 3 + 1] = zero_vertex + 1 + (i + 1) % SAMPLES;
            m_faces[face_counter * 3 + 2] = zero_vertex + 1 + i;
            face_counter++;
        }

        // duplicate top and bottom points (color is different), connect them
        zero_vertex = vertex_counter;
        for (int i = 0; i < SAMPLES; i++)
        {
            const float angle = float(i) / float(SAMPLES) * glm::pi<float>() * 2.0f;
            const float x = radius * std::cos(angle);
            const float y = radius * std::sin(angle);
            const float zt = height / 2.0f;
            const float zb = -height / 2.0f;

            glm::vec3 normal(x,y,0.0f);
            normal = glm::normalize(normal);

            m_vertices[vertex_counter * 3 + 0] = x;
            m_vertices[vertex_counter * 3 + 1] = y;
            m_vertices[vertex_counter * 3 + 2] = zt;

            m_colors[vertex_counter * 3 + 0] = color_side.r;
            m_colors[vertex_counter * 3 + 1] = color_side.g;
            m_colors[vertex_counter * 3 + 2] = color_side.b;

            m_normals[vertex_counter * 3 + 0] = normal.x;
            m_normals[vertex_counter * 3 + 1] = normal.y;
            m_normals[vertex_counter * 3 + 2] = normal.z;
            vertex_counter++;

            m_vertices[vertex_counter * 3 + 0] = x;
            m_vertices[vertex_counter * 3 + 1] = y;
            m_vertices[vertex_counter * 3 + 2] = zb;

            m_colors[vertex_counter * 3 + 0] = color_side.r;
            m_colors[vertex_counter * 3 + 1] = color_side.g;
            m_colors[vertex_counter * 3 + 2] = color_side.b;

            m_normals[vertex_counter * 3 + 0] = normal.x;
            m_normals[vertex_counter * 3 + 1] = normal.y;
            m_normals[vertex_counter * 3 + 2] = normal.z;
            vertex_counter++;

            m_faces[face_counter * 3 + 0] = zero_vertex + i * 2;
            m_faces[face_counter * 3 + 1] = zero_vertex + i * 2 + 1;
            m_faces[face_counter * 3 + 2] = zero_vertex + ((i + 1) % SAMPLES) * 2 + 1;
            face_counter++;

            m_faces[face_counter * 3 + 0] = zero_vertex + i * 2;
            m_faces[face_counter * 3 + 1] = zero_vertex + ((i + 1) % SAMPLES) * 2 + 1;
            m_faces[face_counter * 3 + 2] = zero_vertex + ((i + 1) % SAMPLES) * 2;
            face_counter++;
        }

        m_faces_size = face_counter;
        m_vertices_size = vertex_counter;
    }

    ~CylinderGeometry()
    {
        delete[] m_vertices;
        delete[] m_colors;
        delete[] m_faces;
        delete[] m_normals;
    }

    const GLfloat * vertices() { return m_vertices; }
    const GLfloat * colors() { return m_colors; }
    const GLfloat * normals() { return m_normals; }
    const GLuint * faces() { return m_faces; }
    GLsizei verticesSize() { return m_vertices_size; }
    GLsizei size() { return m_faces_size * 3; }

    GLenum type() { return GL_TRIANGLES; }

private:
    GLfloat * m_vertices;
    GLfloat * m_colors;
    GLfloat * m_normals;
    GLsizei m_vertices_size;

    GLuint * m_faces;
    GLsizei m_faces_size;
};


#endif // CYLINDER_GEOMETRY_H
