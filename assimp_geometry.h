#pragma once

#include <glad/glad.h>

#include "model_renderer.h"

#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class AssimpGeometry : public IGeometry
{
public:
    AssimpGeometry(const std::string & filename)
    {
        m_vertices_size = 0;
        m_faces_size = 0;
        m_any_color = false;
        m_any_texcoord = false;

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs |
                                                 aiProcess_GenNormals);
        // check for errors
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
            return;
        }

        for (unsigned i = 0; i < scene->mNumMeshes; i++)
          loadMesh(scene->mMeshes[i]);

        m_faces_size = m_faces.size() / 3;
        m_vertices_size = m_vertices.size() / 3;
    }

    ~AssimpGeometry()
    {
    }

    void loadMesh(aiMesh *mesh)
    {
        unsigned start = m_vertices.size() / 3;
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            m_vertices.push_back(mesh->mVertices[i].x);
            m_vertices.push_back(mesh->mVertices[i].y);
            m_vertices.push_back(mesh->mVertices[i].z);

            m_normals.push_back(mesh->mNormals[i].x);
            m_normals.push_back(mesh->mNormals[i].y);
            m_normals.push_back(mesh->mNormals[i].z);

            if (mesh->mColors[0])
            {
                m_colors.push_back(mesh->mColors[0][i].r);
                m_colors.push_back(mesh->mColors[0][i].g);
                m_colors.push_back(mesh->mColors[0][i].b);
                m_any_color = true;
            }
            else
            {
                m_colors.push_back(0.0f);
                m_colors.push_back(0.0f);
                m_colors.push_back(0.0f);
            }

            if (mesh->mTextureCoords[0])
            {
                m_texcoords.push_back(mesh->mTextureCoords[0][i].x);
                m_texcoords.push_back(mesh->mTextureCoords[0][i].y);
                m_any_texcoord = true;
            }
            else
            {
                m_texcoords.push_back(0.0f);
                m_texcoords.push_back(0.0f);
            }
        }


        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            if (face.mNumIndices != 3)
                continue;
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                m_faces.push_back(start + face.mIndices[j]);
        }
    }

    const GLfloat * vertices() { return m_vertices.data(); }
    const GLfloat * colors() { return m_any_color ? m_colors.data() : NULL; }
    const GLfloat * normals() { return m_normals.data(); }
    const GLfloat * texCoords() { return m_any_texcoord ? m_texcoords.data() : NULL; }
    const GLuint * faces() { return m_faces.data(); }
    GLsizei verticesSize() { return m_vertices_size; }
    GLsizei size() { return m_faces_size * 3; }

    GLenum type() { return GL_TRIANGLES; }

private:
    std::vector<GLfloat> m_vertices;
    std::vector<GLfloat> m_colors;
    std::vector<GLfloat> m_normals;
    std::vector<GLfloat> m_texcoords;
    GLsizei m_vertices_size;

    bool m_any_color;
    bool m_any_texcoord;

    std::vector<GLuint> m_faces;
    GLsizei m_faces_size;
};
