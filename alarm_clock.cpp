#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>

#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "model_renderer.h"
#include "create_shader_program.h"
#include "compute_normals.h"
#include "load_texture.h"
#include "assimp_geometry.h"
#include "init_window.h"

unsigned int scr_width = 600;
unsigned int scr_height = 600;

int shaderProgram;
GLint transformationUniformLocation;
GLint modelviewUniformLocation;
// light properties
GLint lightPositionUniformLocation;
GLint lightAmbientUniformLocation;
GLint lightDiffuseUniformLocation;
GLint lightSpecularUniformLocation;
// material properties
GLint shininessUniformLocation;
GLint colorSpecularUniformLocation;
GLint colorEmittedUniformLocation;
GLint hasTextureUniformLocation;
GLint isBoxUniformLocation;

GLint colorTextureUniformLocation;

ModelRenderer * box;
ModelRenderer * bell;
ModelRenderer * box_glass;
ModelRenderer * prism;
ModelRenderer * hammer_head;
ModelRenderer * pointer;

float pointer_angle = 0.0f;
float hammer_angle = 0.0f;
int hammer_direction = 1.0f;

bool isHammerMoving;
bool isHammerMovementActivated; //after KEY A is pressed
bool hasBeenKeyAActive;

glm::mat4 inputModelMatrix = glm::mat4(1.0);

void load_matrices(glm::mat4 projection_matrix, glm::mat4 view_matrix, glm::mat4 model_matrix)
{
  glm::mat4 transf = projection_matrix * view_matrix * model_matrix;
  glm::mat4 modelview = view_matrix * model_matrix;
  glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
  glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
}

void display_pointer(glm::mat4 projection_matrix, glm::mat4 view_matrix, glm::mat4 parent_model, GLFWwindow* window)
{
    glm::mat4 pointer_matrix = parent_model;
    pointer_matrix = glm::rotate(pointer_matrix, - pointer_angle ,glm::vec3(0.0f, 0.0f, 1.0f));
    pointer_matrix = glm::translate(pointer_matrix, glm::vec3(0.66f, 0.0f, 1.5f + 0.25f));
    load_matrices(projection_matrix, view_matrix, pointer_matrix);
    pointer->render();
}

void display_hammer(glm::mat4 projection_matrix, glm::mat4 view_matrix, glm::mat4 parent_model, GLFWwindow* window)
{
    glm::mat4 hammer_model_matrix = parent_model;
    hammer_model_matrix = glm::rotate(hammer_model_matrix, hammer_angle, glm::vec3(0.0f, 0.0f, 1.0f));
    
    //hammer body
    glm::mat4 hammer_body_matrix = hammer_model_matrix;
    hammer_body_matrix = glm::translate(hammer_body_matrix, glm::vec3(0.0f, 1.5f, 0.0f));
    hammer_body_matrix = glm::scale(hammer_body_matrix, glm::vec3(0.1f / 2.0f, 3.0f / 2.0f, 0.1f / 2.0f));
    load_matrices(projection_matrix, view_matrix, hammer_body_matrix);
    prism->render();

    //hammer head
    glm::mat4 hammer_head_matrix = hammer_model_matrix;
    hammer_head_matrix = glm::translate(hammer_head_matrix, glm::vec3(0.0f, 3.0f + 0.2f, 0.0f));
    load_matrices(projection_matrix, view_matrix, hammer_head_matrix);
    hammer_head->render();
}

void display_notches(glm::mat4 projection_matrix, glm::mat4 view_matrix, glm::mat4 parent_model, GLFWwindow* window)
{

    glm::mat4 notch_model_matrix = parent_model;

    for (int i = 0; i < 12; i++)
    {
        glm::mat4 notch_matrix = notch_model_matrix;
        float lenght = i % 2 == 0 ? 0.7f : 0.35f;
        float angle = i * (2.0 * glm::pi<float>() / 12); 

        notch_matrix = glm::rotate(notch_matrix, angle, glm::vec3(0.0, 0.0, 1.0));
        notch_matrix = glm::translate(notch_matrix, glm::vec3(2.0f - lenght / 2.0f, 0.0, 1.505f));
        notch_matrix = glm::rotate(notch_matrix, glm::pi<float>() / 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        //scale into prism
        notch_matrix = glm::scale(notch_matrix, glm::vec3(0.1f / 2.0f, 0.1f / 2.0f, lenght / 2.0f));
        load_matrices(projection_matrix, view_matrix, notch_matrix);
        prism->render();
    }

}

void display(GLFWwindow* window)
{
    // render
    // ------
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgram);

    const float PI = std::acos(-1.0f);
    glm::mat4 model_matrix = glm::mat4(1.0);
    model_matrix = inputModelMatrix * model_matrix;

    glm::mat4 view_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -15.0f));

    glm::mat4 projection_matrix = glm::perspective(glm::pi<float>() / 4.0f, float(scr_width) / float(scr_height), 1.0f, 25.0f);

    glm::vec3 light_position(-20.0f, 10.0f, 20.0f);
    glm::vec3 light_camera_position = glm::vec3(view_matrix * glm::vec4(light_position, 1.0f));
    glm::vec3 light_ambient(0.2f, 0.2f, 0.2f);
    glm::vec3 light_diffuse(0.7f, 0.7f, 0.7f);
    glm::vec3 light_specular(0.7f, 0.7f, 0.7f);
    glUniform3fv(lightPositionUniformLocation, 1, glm::value_ptr(light_camera_position));
    glUniform3fv(lightAmbientUniformLocation, 1, glm::value_ptr(light_ambient));
    glUniform3fv(lightDiffuseUniformLocation, 1, glm::value_ptr(light_diffuse));
    glUniform3fv(lightSpecularUniformLocation, 1, glm::value_ptr(light_specular));

    GLfloat shininess = 25.0f;
    glUniform1f(shininessUniformLocation, shininess);
    glUniform3f(colorSpecularUniformLocation, 0.4f, 0.4f, 0.4f);
    glUniform3f(colorEmittedUniformLocation, 0.0f, 0.0f, 0.0f);

    glUniform1i(colorTextureUniformLocation, 0);

    //box
    glUniform1i(isBoxUniformLocation, true);
    glDisable(GL_CULL_FACE); //disable back face culling
    load_matrices(projection_matrix, view_matrix, model_matrix);
    box->render();
    glEnable(GL_CULL_FACE); //enable back face culling
    glUniform1i(isBoxUniformLocation, false);

    //box glass in wideframe mode
    glm::mat4 box_glass_matrix = model_matrix;
    box_glass_matrix = glm::translate(box_glass_matrix, glm::vec3(0.0f, 0.0f, 1.75f));
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //wideframe ON
    load_matrices(projection_matrix, view_matrix, box_glass_matrix);
    box_glass->render();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //wideframe OFF

    //bell right
    glm::mat4 bell_right_matrix = model_matrix;
    // pi/2 - pi/5
    bell_right_matrix = glm::rotate(bell_right_matrix, glm::pi<float>() * 3.0f / 10.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    bell_right_matrix = glm::translate(bell_right_matrix, glm::vec3(2.4f, 0.0f, 0.0f));
    bell_right_matrix = glm::rotate(bell_right_matrix, glm::pi<float>() / 2.0f ,glm::vec3(0.0f, 1.0f, 0.0f));
    load_matrices(projection_matrix, view_matrix, bell_right_matrix);
    bell->render();
    

    //bell left
    glm::mat4 bell_left_matrix = model_matrix;
    // pi/5 - pi/2
    bell_left_matrix = glm::rotate(bell_left_matrix, -glm::pi<float>() *3.0f/ 10.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    bell_left_matrix = glm::translate(bell_left_matrix, glm::vec3(-2.4f, 0.0f, 0.0f));
    bell_left_matrix = glm::rotate(bell_left_matrix, -glm::pi<float>() / 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    load_matrices(projection_matrix, view_matrix, bell_left_matrix);
    bell->render();

    display_notches(projection_matrix, view_matrix, model_matrix, window);

    display_hammer(projection_matrix, view_matrix, model_matrix, window);

    display_pointer(projection_matrix, view_matrix, model_matrix, window);

    glfwSwapBuffers(window);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    scr_width = width;
    scr_height = height;

    glViewport(0, 0, width, height);
    display(window);
}

void window_refresh_callback(GLFWwindow* window)
{
    display(window);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (key == GLFW_KEY_S && action == GLFW_PRESS) {
        isHammerMoving = !isHammerMoving;
        isHammerMovementActivated = false;
    }

    if (key == GLFW_KEY_A && action == GLFW_PRESS)
        isHammerMovementActivated = true;
}

void mouse_cursor_callback(GLFWwindow * window, double xpos, double ypos)
{
    static float prev_x = -1.0; // position at previous iteration (-1 for none)
    static float prev_y = -1.0;
    const float SPEED = 0.005f; // rad/pixel

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        if (prev_x >= 0.0f && prev_y >= 0.0f)   // if there is a previously stored position
        {
            float xdiff = float(xpos) - prev_x; // compute diff
            float ydiff = float(ypos) - prev_y;
            float delta_y = SPEED * ydiff;
            float delta_x = SPEED * xdiff;

            glm::mat4 rot = glm::mat4(1.0);    // rotate matrix
            rot = glm::rotate(rot, delta_x, glm::vec3(0.0, 1.0, 0.0));
            rot = glm::rotate(rot, delta_y, glm::vec3(1.0, 0.0, 0.0));
            inputModelMatrix = rot * inputModelMatrix;
        }

        prev_x = float(xpos); // store mouse position for next iteration
        prev_y = float(ypos);
    }
    else
    {
        prev_x = -1.0f; // mouse released: reset
        prev_y = -1.0f;
    }
}

void advance(GLFWwindow* window, double time_diff)
{
    float delta_x = 0.0;
    float delta_y = 0.0;
    const float speed = 0.5;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        delta_y = -1.0;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        delta_x = -1.0;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        delta_y = 1.0;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        delta_x = 1.0;
    delta_y *= speed * float(time_diff);
    delta_x *= speed * float(time_diff);

    glm::mat4 rot = glm::mat4(1.0);
    rot = glm::rotate(rot, delta_x, glm::vec3(0.0, 1.0, 0.0));
    rot = glm::rotate(rot, delta_y, glm::vec3(1.0, 0.0, 0.0));
    inputModelMatrix = rot * inputModelMatrix;

    const float pointer_speed = 2.0f;
    const float hammer_speed = 3.0f;
    
    //disable pointer rotation if KEY R is pressed
    if (glfwGetKey(window, GLFW_KEY_R) != GLFW_PRESS)
        pointer_angle += time_diff * pointer_speed;

    if (pointer_angle >= 2.0f * glm::pi<float>()) {
        pointer_angle = 0.0f;
        hasBeenKeyAActive = false;
    }

    //run hammer if KEY A has been pressed and pointer angle is grater than 3*pi/2
    if (pointer_angle >= 3.0f * glm::pi<float>() / 2.0f && !hasBeenKeyAActive)
        if (isHammerMovementActivated) {
            isHammerMoving = true;
            hasBeenKeyAActive = true;
        }
        
    
    if (isHammerMoving)
        hammer_angle += hammer_direction * time_diff * hammer_speed;

    if (hammer_angle >= glm::pi<float>() / 12.0f)
        hammer_direction = -1.0f;

    if (hammer_angle <= -glm::pi<float>() / 12.0f)
        hammer_direction = 1.0f;
}

#include "cylinder_geometry.h"
#include "sphere_geometry.h"

class CubeGeometry : public IGeometry
{
public:
    CubeGeometry()
    {
    }

    ~CubeGeometry()
    {
    }

    const GLfloat* vertices()
    {
        static GLfloat avertices[] = {
            -1.0,  -1.0,   1.0,
             1.0,  -1.0,   1.0,
             1.0,   1.0,   1.0,
            -1.0,   1.0,   1.0,

             1.0,  -1.0,   1.0,
             1.0,  -1.0,  -1.0,
             1.0,   1.0,  -1.0,
             1.0,   1.0,   1.0,

            -1.0,   1.0,   1.0,
             1.0,   1.0,   1.0,
             1.0,   1.0,  -1.0,
            -1.0,   1.0,  -1.0,

            -1.0,  -1.0,   1.0,
            -1.0,  -1.0,  -1.0,
            -1.0,   1.0,  -1.0,
            -1.0,   1.0,   1.0,

            -1.0,  -1.0,   1.0,
             1.0,  -1.0,   1.0,
             1.0,  -1.0,  -1.0,
            -1.0,  -1.0,  -1.0,

            -1.0,  -1.0,  -1.0,
             1.0,  -1.0,  -1.0,
             1.0,   1.0,  -1.0,
            -1.0,   1.0,  -1.0,
        };

        return avertices;
    }
    const GLfloat* colors()
    {
        static GLfloat acolors[] = {
            0.2,   0.2,   0.2,
            0.2,   0.2,   0.2,
            0.2,   0.2,   0.2,
            0.2,   0.2,   0.2,

            0.2,   0.2,   0.2,
            0.2,   0.2,   0.2,
            0.2,   0.2,   0.2,
            0.2,   0.2,   0.2,

            0.2,   0.2,   0.2,
            0.2,   0.2,   0.2,
            0.2,   0.2,   0.2,
            0.2,   0.2,   0.2,

            0.2,   0.2,   0.2,
            0.2,   0.2,   0.2,
            0.2,   0.2,   0.2,
            0.2,   0.2,   0.2,

            0.2,   0.2,   0.2,
            0.2,   0.2,   0.2,
            0.2,   0.2,   0.2,
            0.2,   0.2,   0.2,

            0.2,   0.2,   0.2,
            0.2,   0.2,   0.2,
            0.2,   0.2,   0.2,
            0.2,   0.2,   0.2,
        };

        return acolors;
    }

    const GLfloat* normals()
    {
        static GLfloat anormals[] = {
            // front
             0.0,   0.0,   1.0,
             0.0,   0.0,   1.0,
             0.0,   0.0,   1.0,
             0.0,   0.0,   1.0,
             // right
              1.0,   0.0,   0.0,
              1.0,   0.0,   0.0,
              1.0,   0.0,   0.0,
              1.0,   0.0,   0.0,
              // top
               0.0,   1.0,   0.0,
               0.0,   1.0,   0.0,
               0.0,   1.0,   0.0,
               0.0,   1.0,   0.0,
               // left
               -1.0,   0.0,   0.0,
               -1.0,   0.0,   0.0,
               -1.0,   0.0,   0.0,
               -1.0,   0.0,   0.0,
               // bottom
                0.0,  -1.0,   0.0,
                0.0,  -1.0,   0.0,
                0.0,  -1.0,   0.0,
                0.0,  -1.0,   0.0,
                // back
                 0.0,   0.0,  -1.0,
                 0.0,   0.0,  -1.0,
                 0.0,   0.0,  -1.0,
                 0.0,   0.0,  -1.0,
        };

        return anormals;
    }

    GLsizei verticesSize() { return 24; }
    const GLuint* faces()
    {
        static unsigned int indices[] = {
            0, 1, 2, // front
            0, 2, 3, // front

            4, 5, 6, // right
            4, 6, 7, // right

            8, 9,10, // top
            8,10,11, // top

           12,14,13, // left
           12,15,14, // left

           16,18,17, // bottom
           16,19,18, // bottom

           20,23,21, // back
           21,23,22, // back
        };

        return indices;
    }

    GLsizei size() { return 36; }

    GLenum type() { return GL_TRIANGLES; }

private:
};

int main()
{
    GLFWwindow * window = init_window(scr_width, scr_height, "Car Texture");

    // callbacks
    // ---------
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetWindowRefreshCallback(window, window_refresh_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_cursor_callback);

    AssimpGeometry bell_geo("src/p24_bell.ply");
    ModelRenderer bell_geo_renderer(bell_geo);
    bell = &bell_geo_renderer;

    AssimpGeometry pointer_geo("src/p24_hand.ply");
    ModelRenderer pointer_geo_renderer(pointer_geo);
    pointer = &pointer_geo_renderer;

    CylinderGeometry box_geo(2.4f, 3.0f, glm::vec3(0.7f, 0.7f, 0.7f)
                                       , glm::vec3(1.0f, 0.7f, 0.0f)
                                       , glm::vec3(1.0f, 0.7f, 0.0f));
    ModelRenderer box_geo_renderer(box_geo);
    box = &box_geo_renderer;

    CylinderGeometry box_glass_geo(2.4f, 0.5f, glm::vec3(1.0f)
                                             , glm::vec3(1.0f)
                                             , glm::vec3(1.0f));
    ModelRenderer box_glass_geo_renderer(box_glass_geo);
    box_glass = &box_glass_geo_renderer;

    CubeGeometry prism_geo;
    ModelRenderer prism_geo_renderer(prism_geo);
    prism = &prism_geo_renderer;

    SphereGeometry hammer_head_geo(0.2f, glm::vec3(1.0f));
    ModelRenderer hammer_head_geo_renderer(hammer_head_geo);
    hammer_head = &hammer_head_geo_renderer;

    // load GLSL shaders
    shaderProgram = createShaderProgram("alarm_clock.vert", "alarm_clock.frag");
    transformationUniformLocation = glGetUniformLocation(shaderProgram, "transformation");
    modelviewUniformLocation = glGetUniformLocation(shaderProgram, "modelview");
 
    lightPositionUniformLocation = glGetUniformLocation(shaderProgram, "light_position");
    lightAmbientUniformLocation = glGetUniformLocation(shaderProgram, "light_ambient");
    lightDiffuseUniformLocation = glGetUniformLocation(shaderProgram, "light_diffuse");
    lightSpecularUniformLocation = glGetUniformLocation(shaderProgram, "light_specular");

    shininessUniformLocation = glGetUniformLocation(shaderProgram, "shininess");
    colorSpecularUniformLocation = glGetUniformLocation(shaderProgram, "color_specular");
    colorEmittedUniformLocation = glGetUniformLocation(shaderProgram, "color_emitted");

    colorTextureUniformLocation = glGetUniformLocation(shaderProgram, "color_texture");

    hasTextureUniformLocation = glGetUniformLocation(shaderProgram, "has_texture");
    isBoxUniformLocation = glGetUniformLocation(shaderProgram, "isBox");

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // enable back face culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // enable depth test
    glEnable(GL_DEPTH_TEST);

    // render loop
    // -----------
    double curr_time = glfwGetTime();
    double prev_time;
    while (!glfwWindowShouldClose(window))
    {
        display(window);
        glfwWaitEventsTimeout(0.01);

        prev_time = curr_time;
        curr_time = glfwGetTime();
        advance(window, curr_time - prev_time);
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}
