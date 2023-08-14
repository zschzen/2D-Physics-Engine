#include "Application.h"

#include "./Physics/Constants.h"
#include "./Physics/Force.h"

bool Application::IsRunning() {
    return running;
}

///////////////////////////////////////////////////////////////////////////////
// Setup function (executed once in the beginning of the simulation)
///////////////////////////////////////////////////////////////////////////////
void Application::Setup() {
    running = Graphics::OpenWindow();

    anchor = Vec2(Graphics::Width() / 2.0, 30);

    // Create particles in a chain
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        Particle* particle = new Particle(anchor.x, anchor.y + (i * restLength), 2.0);
        particle->radius = 5;
        particle->color = 0xFF000000 | (rand() % 0xFFFFFF);
        particles.push_back(particle);
    }
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

                    // Get nearest particle to the mouse cursor
                    selectedParticle = FindClosestParticle(Vec2(x, y));

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
    if (deltaTime > 0.016) deltaTime = 0.016;

    // Set the time of the current frame to be used in the next one
    timePreviousFrame = SDL_GetTicks();

    // Add forces to the particles
    for (const auto &particle: particles) {
        //particle->AddForce(Vec2(4.0f, 0.0f) * PIXELS_PER_METER);
        //particle->AddForce(Vec2(0.0f, particle->mass * GRAVITY) * PIXELS_PER_METER);
        particle->AddForce(pushForce);

        // Calculate drag force
        Vec2 drag = Force::GenerateDragForce(*particle, 0.001);
        particle->AddForce(drag);
        
        // Weight force
        Vec2 weight = Vec2(0.0f, particle->mass * GRAVITY * PIXELS_PER_METER);
        particle->AddForce(weight);
    }

    // Apply a spring force to the particle connected to the anchor
    Vec2 springForce = Force::GenerateSpringForce(*particles[0], anchor, restLength, k);
    particles[0]->AddForce(springForce);
    
    // Apply a spring force to the particles connected to each other
    for (int i = 1; i < PARTICLE_COUNT; i++) {
        Vec2 springForce = Force::GenerateSpringForce(*particles[i], *particles[i - 1], restLength, k);
        particles[i]->AddForce(springForce);
        particles[i - 1]->AddForce(-springForce);
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
    Graphics::ClearScreen(0xFF0F0721);

    if (selectedParticle) {
        Graphics::DrawLine(selectedParticle->position.x, selectedParticle->position.y, mouseCursor.x, mouseCursor.y, 0xFF0000FF);
    }

    // Draw the spring
    Graphics::DrawLine(particles[0]->position.x, particles[0]->position.y, anchor.x, anchor.y, 0xFF00FF00);
    for (int i = 1; i < PARTICLE_COUNT; i++) {
        Graphics::DrawLine(particles[i]->position.x, particles[i]->position.y, particles[i - 1]->position.x, particles[i - 1]->position.y, 0xFF00FF00);
    }

    // Draw the anchor
    Graphics::DrawFillCircle(anchor.x, anchor.y, 5, 0xFF001155);
    
    for (const auto &particle: particles) {
        Graphics::DrawFillCircle(particle->position.x, particle->position.y, particle->radius, particle->color);
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

Particle *Application::FindClosestParticle(const Vec2 &position)
{
    Particle *closestParticle = nullptr;
    float closestDistance = 0.0f;

    for (const auto &particle: particles) {
        float distance = (particle->position - position).Magnitude();
        if (closestParticle && distance >= closestDistance) continue;

        closestParticle = particle;
        closestDistance = distance;
    }

    return closestParticle;
}