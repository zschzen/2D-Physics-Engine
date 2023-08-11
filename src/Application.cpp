#include "Application.h"

#include "./Physics/Constants.h"
#include "./Physics/Force.h"

#include <cstdlib>

bool Application::IsRunning() {
    return running;
}

///////////////////////////////////////////////////////////////////////////////
// Setup function (executed once in the beginning of the simulation)
///////////////////////////////////////////////////////////////////////////////
void Application::Setup() {
    running = Graphics::OpenWindow();

    auto particle = new Particle(200, 100, 5.0f);
    particle->radius = 25;
    particles.push_back(particle);

    particle = new Particle(50, 100, 1.0f);
    particle->radius = 5;
    particles.push_back(particle);
}

///////////////////////////////////////////////////////////////////////////////
// Input processing
///////////////////////////////////////////////////////////////////////////////
void Application::Input() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    running = false;
                if (event.key.keysym.sym == SDLK_UP)
                    pushForce.y = -50 * PIXELS_PER_METER;
                if (event.key.keysym.sym == SDLK_RIGHT)
                    pushForce.x = 50 * PIXELS_PER_METER;
                if (event.key.keysym.sym == SDLK_DOWN)
                    pushForce.y = 50 * PIXELS_PER_METER;
                if (event.key.keysym.sym == SDLK_LEFT)
                    pushForce.x = -50 * PIXELS_PER_METER;
                break;
            case SDL_KEYUP:
                if (event.key.keysym.sym == SDLK_UP)
                    pushForce.y = 0;
                if (event.key.keysym.sym == SDLK_RIGHT)
                    pushForce.x = 0;
                if (event.key.keysym.sym == SDLK_DOWN)
                    pushForce.y = 0;
                if (event.key.keysym.sym == SDLK_LEFT)
                    pushForce.x = 0;
                break;
            case SDL_MOUSEMOTION:
                mouseCursor.x = event.motion.x;
                mouseCursor.y = event.motion.y;
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (!selectedParticle && event.button.button == SDL_BUTTON_LEFT) {
                    int x, y;
                    SDL_GetMouseState(&x, &y);

                    // Determine if the mouse cursor is over a particle
                    for (auto particle : particles) {
                        if ((particle->position - Vec2(x, y)).Magnitude() >= particle->radius) continue;
                        selectedParticle = particle;
                        break;
                    }

                    mouseCursor.x = x;
                    mouseCursor.y = y;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (selectedParticle && event.button.button == SDL_BUTTON_LEFT) {
                    Vec2 impulseDirection = (selectedParticle->position - mouseCursor).UnitVector();
                    float impulseMagnitude = (selectedParticle->position - mouseCursor).Magnitude() * 5.0;
                    selectedParticle->velocity = impulseDirection * impulseMagnitude;
                    selectedParticle = nullptr;
                }
                break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Update function (called several times per second to update objects)
///////////////////////////////////////////////////////////////////////////////
void Application::Update() {
    // Wait some time until the reach the target frame time in milliseconds
    static int timePreviousFrame;
    int timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks() - timePreviousFrame);
    if (timeToWait > 0) {
#if __EMSCRIPTEN__
        emscripten_sleep(timeToWait);
#else
        SDL_Delay(timeToWait);
#endif
    }

    // Calculate the deltatime in seconds
    float deltaTime = (SDL_GetTicks() - timePreviousFrame) / 1000.0f;
    if (deltaTime > MILLISECS_PER_FRAME) deltaTime = MILLISECS_PER_FRAME;

    // Set the time of the current frame to be used in the next one
    timePreviousFrame = SDL_GetTicks();

    // Add forces to the particles
    for (const auto &particle: particles) {
        //particle->AddForce(Vec2(4.0f, 0.0f) * PIXELS_PER_METER);
        //particle->AddForce(Vec2(0.0f, particle->mass * GRAVITY) * PIXELS_PER_METER);
        particle->AddForce(pushForce);

        // Calculate friction
        Vec2 friction = Force::GenerateFrictionForce(*particle, 15 * PIXELS_PER_METER);
        particle->AddForce(friction);
    }

    // Update the particles integration
    for (const auto &particle: particles) {
        particle->Integrate(deltaTime);
    }

    // Keep particles inside the screen
    for (const auto &particle: particles) {
        // Left
        if (particle->position.x - particle->radius < 0) {
            particle->position.x = particle->radius;
            particle->velocity.x *= -0.9;
        }

        // Right
        if (particle->position.x + particle->radius > Graphics::windowWidth) {
            particle->position.x = Graphics::windowWidth - particle->radius;
            particle->velocity.x *= -0.9;
        }

        // Top
        if (particle->position.y - particle->radius < 0) {
            particle->position.y = particle->radius;
            particle->velocity.y *= -0.9;
        }

        // Bottom
        if (particle->position.y + particle->radius > Graphics::windowHeight) {
            particle->position.y = Graphics::windowHeight - particle->radius;
            particle->velocity.y *= -0.9;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Render function (called several times per second to draw objects)
///////////////////////////////////////////////////////////////////////////////
void Application::Render() {
    Graphics::ClearScreen(0xFF1E4002);

    if (selectedParticle) {
        Graphics::DrawLine(selectedParticle->position.x, selectedParticle->position.y, mouseCursor.x, mouseCursor.y, 0xFF0000FF);
    }

    for (const auto &particle: particles) {
        Graphics::DrawFillCircle(particle->position.x, particle->position.y, particle->radius, 0xFFFFFFFF);
    }
    Graphics::RenderFrame();
}

///////////////////////////////////////////////////////////////////////////////
// Destroy function to delete objects and close the window
///////////////////////////////////////////////////////////////////////////////
void Application::Destroy() {
    for (const auto &particle: particles) {
        delete particle;
    }

    Graphics::CloseWindow();
}