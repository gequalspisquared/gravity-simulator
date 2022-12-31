#ifndef SIM_H
#define SIM_H

#include <string>
#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Entity.h"
#include "EntityManager.h"
#include "Vec3.h"

struct PlanetConfig
{
    ncp::Vec3 pos, vel;
    double    mass;
};

namespace ncp
{
    class Sim
    {
    public: 
        Sim(const std::string &windowName, const std::string &config);

        void run();

    private:
        GLFWwindow    *m_window;
        EntityManager  m_entities;
        bool           m_paused = false;
        bool           m_running = true;

        void init(const std::string &windowName, const std::string &config);
        void setPaused(bool paused);

        void sMovement();
        void sUserInput();
        void sRender();

        void sSpawnPlanets();
    };
}

#endif