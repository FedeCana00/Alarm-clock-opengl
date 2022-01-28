#version 330 core
uniform vec3 light_position;
uniform vec3 light_ambient;
uniform vec3 light_diffuse;
uniform vec3 light_specular;

uniform float shininess;
uniform vec3 color_specular;
uniform vec3 color_emitted;

uniform sampler2D color_texture;
uniform bool has_texture;
uniform bool isBox;

in vec2 vTexCoords;
in vec3 vNormal;
in vec3 vPosition;
in vec3 vModelPosition;
in vec3 vColor;

out vec4 FragColor;

void main()
{
   vec3 color;
   if (has_texture)
     color = texture(color_texture, vTexCoords).rgb;
   else
     color = vColor;

   if (isBox &&
     (vModelPosition.z*vModelPosition.z + vModelPosition.x*vModelPosition.x) < 2.0f*3.14f /12.0f
     && vModelPosition.y >= 0)
     discard;

   if (!gl_FrontFacing)
   {
     color = vec3(1, 0.0, 0.0);
     FragColor = vec4(clamp(color, 0.0, 1.0), 1.0);
     return;
   }
	 
   vec3 relative_light_pos = light_position - vPosition;
   vec3 normal = normalize(vNormal);
   
   vec3 ambient = color * light_ambient;
   
   float diffuse_intensity = max(0.0, dot(normalize(relative_light_pos), normal));
   vec3 diffuse = diffuse_intensity * color * light_diffuse;
   
   vec3 reflection = reflect(normalize(-relative_light_pos), normal);
   float specular_intensity = pow(max(0.0, dot(reflection, normalize(-vPosition))), shininess);
   vec3 specular = specular_intensity * color_specular * light_specular;
   
   vec3 emitted = color_emitted;
   
   FragColor = vec4(clamp(ambient + diffuse + specular + emitted, 0.0, 1.0), 1.0);
}
