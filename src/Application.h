#ifndef APPLICATION_H
#define APPLICATION_H

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define DELAY(timeToWait) emscripten_sleep(timeToWait)
#else
#define DELAY(timeToWait) SDL_Delay(timeToWait)
#endif


#include "./Graphics.h"
#include "./Physics/World.h"

class Application {
    private:
        bool running = false;
		
		World* world = nullptr;
        
        bool Debug = true;

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