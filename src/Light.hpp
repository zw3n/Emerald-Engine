#ifndef LIGHT_H
#define LIGHT_H

#ifdef __linux__
  #include <GL/glew.h>
#endif

#ifdef __APPLE__
  #include <OpenGL/gl3.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <string>
#include <vector>

class Light
{
public:
    static void upload_all();
    static GLuint shader_program;

    Light(const glm::vec3 world_coord, const glm::vec3 color);
    ~Light();

    void upload();
    glm::vec3 get_color();
    void set_color(glm::vec3 color);
    void move_to(glm::vec3 world_coord); // does not upload data
    void upload_pos();

private:
    unsigned int id;
    glm::vec3 position, color;
    GLboolean active_light;

    static std::vector<Light*> lights;
    static std::vector<unsigned int> free_ids;
};



#endif
