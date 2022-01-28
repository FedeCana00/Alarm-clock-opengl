#ifndef SPHERE_GEOMETRY_H
#define SPHERE_GEOMETRY_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "model_renderer.h"
#include "compute_normals.h"

class SphereGeometry : public IGeometry
{
public:
    SphereGeometry(float radius, glm::vec3 color)
    {
        const int SAMPLES_LAT = 24;
        const int SAMPLES_LON = 24;
        m_vertices = new GLfloat[3 * ((SAMPLES_LAT - 2) * SAMPLES_LON + 2)];
        m_colors = new GLfloat[3 * ((SAMPLES_LAT - 2) * SAMPLES_LON + 2)];
        m_normals = new GLfloat[3 * ((SAMPLES_LAT - 2) * SAMPLES_LON + 2)];
        m_faces = new GLuint[3 * (2 * (SAMPLES_LAT - 3) + 2) * SAMPLES_LON];

        int face_counter = 0;
        int vertex_counter = 0;

        // generate vertices
        for (int vi = 1; vi < SAMPLES_LAT - 1; vi++)
            for (int hi = 0; hi < SAMPLES_LON; hi++)
            {
                float angle_lon = float(hi) / float(SAMPLES_LON) * glm::pi<float>() * 2.0f;
                float angle_lat = float(vi) / float(SAMPLES_LAT - 1) * glm::pi<float>() - glm::pi<float>() / 2.0f;

                glm::mat4 transf = glm::mat4(1.0f);
                transf = glm::rotate(transf, angle_lon, glm::vec3(0.0f, 0.0f, 1.0f));
                transf = glm::rotate(transf, angle_lat, glm::vec3(0.0f, 1.0f, 0.0f));
                glm::vec4 opt = transf * glm::vec4(radius, 0.0f, 0.0f, 1.0f);
                glm::vec3 pt = glm::vec3(opt) / opt.w;

                glm::vec3 normal = glm::normalize(pt);

                m_vertices[vertex_counter * 3 + 0] = pt.x;
                m_vertices[vertex_counter * 3 + 1] = pt.y;
                m_vertices[vertex_counter * 3 + 2] = pt.z;

                m_colors[vertex_counter * 3 + 0] = color.r;
                m_colors[vertex_counter * 3 + 1] = color.g;
                m_colors[vertex_counter * 3 + 2] = color.b;

                m_normals[vertex_counter * 3 + 0] = normal.x;
                m_normals[vertex_counter * 3 + 1] = normal.y;
                m_normals[vertex_counter * 3 + 2] = normal.z;
                vertex_counter++;
            }

        // bottom vertex
        m_vertices[vertex_counter * 3 + 0] = 0.0f;
        m_vertices[vertex_counter * 3 + 1] = 0.0f;
        m_vertices[vertex_counter * 3 + 2] = -radius;

        m_colors[vertex_counter * 3 + 0] = color.r;
        m_colors[vertex_counter * 3 + 1] = color.g;
        m_colors[vertex_counter * 3 + 2] = color.b;

        m_normals[vertex_counter * 3 + 0] = 0.0f;
        m_normals[vertex_counter * 3 + 1] = 0.0f;
        m_normals[vertex_counter * 3 + 2] = -1.0f;
        int bottom_vertex_index = vertex_counter;
        vertex_counter++;

        // top vertex
        m_vertices[vertex_counter * 3 + 0] = 0.0f;
        m_vertices[vertex_counter * 3 + 1] = 0.0f;
        m_vertices[vertex_counter * 3 + 2] = radius;

        m_colors[vertex_counter * 3 + 0] = color.r;
        m_colors[vertex_counter * 3 + 1] = color.g;
        m_colors[vertex_counter * 3 + 2] = color.b;

        m_normals[vertex_counter * 3 + 0] = 0.0f;
        m_normals[vertex_counter * 3 + 1] = 0.0f;
        m_normals[vertex_counter * 3 + 2] = 1.0f;
        int top_vertex_index = vertex_counter;
        vertex_counter++;

        // generate faces
        // top triangles
        for (int hi = 0; hi < SAMPLES_LON; hi++)
        {
            m_faces[face_counter * 3 + 0] = top_vertex_index;
            m_faces[face_counter * 3 + 1] = hi;
            m_faces[face_counter * 3 + 2] = (hi + 1) % SAMPLES_LON;
            face_counter++;
        }

        // quad faces
        for (int vi = 1; vi < SAMPLES_LAT - 2; vi++)
            for (int hi = 0; hi < SAMPLES_LON; hi++)
            {
                m_faces[face_counter * 3 + 0] = (vi - 1) * SAMPLES_LON + hi;
                m_faces[face_counter * 3 + 1] = (vi + 0) * SAMPLES_LON + hi;
                m_faces[face_counter * 3 + 2] = (vi + 0) * SAMPLES_LON + (hi + 1) % SAMPLES_LON;
                face_counter++;

                m_faces[face_counter * 3 + 0] = (vi - 1) * SAMPLES_LON + hi;
                m_faces[face_counter * 3 + 1] = (vi + 0) * SAMPLES_LON + (hi + 1) % SAMPLES_LON;
                m_faces[face_counter * 3 + 2] = (vi - 1) * SAMPLES_LON + (hi + 1) % SAMPLES_LON;
                face_counter++;
            }

        // bottom triangles
        for (int hi = 0; hi < SAMPLES_LON; hi++)
        {
            m_faces[face_counter * 3 + 0] = (SAMPLES_LAT - 3) * SAMPLES_LON + hi;
            m_faces[face_counter * 3 + 1] = bottom_vertex_index;
            m_faces[face_counter * 3 + 2] = (SAMPLES_LAT - 3) * SAMPLES_LON + (hi + 1) % SAMPLES_LON;
            face_counter++;
        }

        m_faces_size = face_counter;
        m_vertices_size = vertex_counter;
    }

    ~SphereGeometry()
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


#endif // SPHERE_GEOMETRY_H
