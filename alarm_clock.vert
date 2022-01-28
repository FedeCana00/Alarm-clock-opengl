#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aTexCoords;

uniform mat4 transformation;
uniform mat4 modelview;

uniform bool has_texture;

out vec2 vTexCoords;
out vec3 vNormal;
out vec3 vPosition;
out vec3 vModelPosition; // posizione in coordinate modello,
out vec3 vColor;
void main()
{
  vModelPosition = aPos; // coordinate modello non trasformate
   gl_Position = transformation * vec4(aPos, 1.0);
   vec4 position = modelview * vec4(aPos, 1.0);
   vPosition = position.xyz / position.w;
   mat3 normal_matrix = transpose(inverse(mat3(modelview)));
   vNormal = normal_matrix * aNormal;
   if (has_texture)
     vTexCoords = aTexCoords;
   else
     vColor = aColor;
}