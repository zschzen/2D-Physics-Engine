#include "Application.h"

Application app;

void AppLoop() {
    while (app.IsRunning()) {
        app.Input();
        app.Update();
        app.Render();
    }
}

int main(int argc, char *args[]) {
    app.Setup();

#if __EMSCRIPTEN__
    emscripten_set_main_loop(AppLoop, 0, 1);
#else
    AppLoop();
#endif

    app.Destroy();

    return 0;
}