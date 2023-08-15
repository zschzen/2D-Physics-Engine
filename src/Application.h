#ifndef APPLICATION_H
#define APPLICATION_H

#if __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "./Graphics.h"
#include "./Physics/Body.h"

class Application {
    private:
        bool running = false;

        std::vector<Body*> particles = std::vector<Body*>();
        Vec2 pushForce{};

        Vec2 mouseCursor = Vec2(0, 0);
        Body* selectedParticle = nullptr;

        Vec2 anchor{};
        float k = 300;
        float restLength = 15;

        const int PARTICLE_COUNT = 10;

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
    Body* FindClosestParticle(const Vec2& position);
};

#endif