#ifndef INPUT_H
#define INPUT_H

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <SDL2/SDL.h>
#include <iostream>
#include "Renderer.hpp"
#include "Camera.hpp"


void init_input();
void handle_keyboard_input(Camera &camera, Renderer &renderer, glm::vec3 &dir);
void handle_mouse_input(Camera &camera);

#endif
