#ifndef APPLICATION_H
#define APPLICATION_H

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define DELAY(timeToWait) emscripten_sleep(timeToWait)
#else
#define DELAY(timeToWait) SDL_Delay(timeToWait)
#endif


#include "./Graphics.h"
#include "./Physics/Body.h"
#include "./Physics/Contact.h"

class Application {
    private:
        bool running = false;

        std::vector<Body*> bodies = std::vector<Body*>();
        std::vector<Contact> contacts = std::vector<Contact>();
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