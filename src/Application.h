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

        float k = 1500;
        float restLength = 200;
        const int SPRING_ROWS = 5;
        const int SPRING_COLS = 2;

    public:
        Application() = default;
        ~Application() = default;
        bool IsRunning();
        void Setup();
        void Input();
        void Update();
        void Render();
        void Destroy();

private:
    Particle* FindClosestParticle(const Vec2& position);

    void CreateGridSoftBody(float x, float y, float mass);
    void ApplySpringForces();
};

#endif