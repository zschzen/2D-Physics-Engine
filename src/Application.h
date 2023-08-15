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

        std::vector<Body*> bodies = std::vector<Body*>();
        Vec2 pushForce{};

        Vec2 mouseCursor = Vec2(0, 0);
        Body* selectedBody = nullptr;

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
    Body* FindClosestBody(const Vec2& position);
};

#endif