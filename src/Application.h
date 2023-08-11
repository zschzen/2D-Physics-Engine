#ifndef APPLICATION_H
#define APPLICATION_H

#if __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "./Graphics.h"
#include "./Physics/Particle.h"

class Application {
    private:
        bool running = false;

        std::vector<Particle*> particles = std::vector<Particle*>();
        Vec2 pushForce{};

        Vec2 mouseCursor = Vec2(0, 0);
        Particle* selectedParticle = nullptr;

    public:
        Application() = default;
        ~Application() = default;
        bool IsRunning();
        void Setup();
        void Input();
        void Update();
        void Render();
        void Destroy();
};

#endif