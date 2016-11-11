#include "main.hpp"


void free_resources()
{
    sdl_quit(main_window, main_context);

}

// --------------------------

void cull_models()
{
    Profiler::start_timer("Cull models");
    // TODO: Run in parallel
    uint models_drawn = 0;
    uint drawn_meshes = 0;
    for (auto model : Model::get_loaded_models()) {
        bool draw_me = camera.sphere_in_frustum(model->get_center_point_world(), \
                model->bounding_sphere_radius * model->scale);
        model->draw_me = draw_me;
       
       // If draw me - see if we can cull meshes
       if (draw_me){
           models_drawn++;
           for (auto mesh : model->get_meshes()) {
               bool draw_me = camera.sphere_in_frustum(mesh->get_center_point_world(model->m2w_matrix), \
                       mesh->bounding_sphere_radius * model->scale);
               mesh->draw_me = draw_me;
               if (draw_me)
                  drawn_meshes++; 
           }
       }
    }

    // Flat models
    for (auto model : Model::get_loaded_flat_models()) {
        bool draw_me = camera.sphere_in_frustum(model->get_center_point_world(), model->bounding_sphere_radius * model->scale);
        model->draw_me = draw_me;
        if (draw_me)
            models_drawn++;
    }

    // Terrain
    for (auto terrain : Terrain::get_loaded_terrain()) {
        bool draw_me = camera.sphere_in_frustum(terrain->get_center_point_world(), terrain->bounding_sphere_radius);
        terrain->draw_me = draw_me;
        // If draw me - see if we can cull meshes
        if (draw_me){
            models_drawn++;
            for (auto mesh : terrain->get_meshes()) {
                bool draw_me = camera.sphere_in_frustum(mesh->get_center_point_world(terrain->m2w_matrix), \
                        mesh->bounding_sphere_radius);
                mesh->draw_me = draw_me;
                if (draw_me)
                    drawn_meshes++; 
            }
        }
    }

    renderer.objects_drawn = models_drawn;
    renderer.meshes_drawn = drawn_meshes;
    Profiler::stop_timer("Cull models");
}

void cull_turned_off_flat_objects()
{
    for (auto model: Model::get_loaded_flat_models()) {
        if (!model->get_light_active()) {
            model->draw_me = false;
        }
    }
}

// --------------------------

void animate_models()
{
    Profiler::start_timer("Animate models");
    // TODO: Run in parallel
    float speed = 0.002;
    for (auto model : Model::get_loaded_models()) {
        if (model->has_animation_path()) {
            model->move_along_path(renderer.get_time_diff()*speed);
        }
    }
    for (auto model : Model::get_loaded_flat_models()) {
        if (model->has_animation_path()) {
            model->move_along_path(renderer.get_time_diff()*speed);
        }
    }
    Profiler::stop_timer("Animate models");
}

// --------------------------

void culling()
{
    cull_models();
    Light::cull_light_sources(camera);
    Light::upload_lights();
    cull_turned_off_flat_objects();
}

// --------------------------

void handle_input()
{
    Profiler::start_timer("Input");
    handle_keyboard_input(camera, renderer);
    handle_mouse_input(camera, renderer);
    Profiler::stop_timer("Input");
}

// --------------------------

void update_camera()
{
    Profiler::start_timer("Camera");
    if (!camera.can_move_free()) {
        camera.move_along_path(0.1f);
    }
    if (!camera.can_look_free()) {
        camera.move_look_point_along_path(0.1f);
    }

    camera.update_culling_frustum();
    camera.update_view_matrix();

    renderer.copy_tweak_bar_cam_values(camera);
    Profiler::stop_timer("Camera");
}

// --------------------------

void run()
{
    renderer.running = true;
    while (renderer.running) {
        Profiler::start_timer("-> Frame time");

        handle_input();

        update_camera();

        animate_models();

        culling();

        renderer.render(camera);

        Profiler::start_timer("Swap");
        SDL_GL_SwapWindow(main_window);
        Profiler::stop_timer("Swap");
        Profiler::stop_timer("-> Frame time");
    }
}


// --------------------------

void print_welcome()
{
    std::string welcome;
    welcome = std::string("This is Emerald Engine.\n");
    welcome += std::string("A few useful commmands are:\n\n");
    welcome += std::string("Numbers 1-7 = Display different buffers\n");
    welcome += std::string("F           = Follow path with camera\n");
    welcome += std::string("T           = Toggle tweakbar display\n");
    welcome += std::string("X,Z         = Interact with lights\n");
    welcome += std::string("P           = Print profiling numbers\n");
    welcome += std::string("\nA complete description of all keyboard commands can be found in doc/keyboard_command_reference.md\n");
    std::cout << welcome.c_str() << std::endl;
}

// --------------------------


void init(int argc, char *argv[])
{
    if (!sdl_init(SCREEN_WIDTH, SCREEN_HEIGHT, main_window, main_context)) {
        Error::throw_error(Error::display_init_fail);
    }
    init_input();

    print_welcome();

    renderer.init();
    renderer.init_uniforms(camera);

    Loader::load_scene(Parser::get_scene_file_from_command_line(argc, argv), &camera);
    renderer.init_tweak_bar(&camera);

    Light::init();
}

// --------------------------

int main(int argc, char *argv[])
{
    init(argc, argv);
    run();

    TwTerminate();
    free_resources();
    return 0;
}
