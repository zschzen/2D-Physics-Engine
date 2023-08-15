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

    // Create soft body particles
    CreateGridSoftBody(400, 300, 1);
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
                    float impulseMagnitude = (selectedParticle->position - mouseCursor).Magnitude() * 15.0;

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
        particle->AddForce(pushForce);

        // Calculate drag force
        Vec2 drag = Force::GenerateDragForce(*particle, 0.001);
        particle->AddForce(drag);
        
        // Weight force
        Vec2 weight = Vec2(0.0f, particle->mass * GRAVITY * PIXELS_PER_METER);
        particle->AddForce(weight);
    }

    // Attach particles with springs
    ApplySpringForces();

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
        } else if (particle->position.x + particle->radius > Graphics::windowWidth) {
            particle->position.x = Graphics::windowWidth - particle->radius;
            particle->velocity.x *= -0.9;
        }

        // Top
        if (particle->position.y - particle->radius < 0) {
            particle->position.y = particle->radius;
            particle->velocity.y *= -0.9;
        } else if (particle->position.y + particle->radius > Graphics::windowHeight) {
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

    // Draw all springs lines
    // 0 0 connects to 0 1, 1 0, 1 1
    for (int i = 0; i < SPRING_ROWS; i++) {
        for (int j = 0; j < SPRING_COLS; j++) {
            int index = i * SPRING_COLS + j;
            
            if (j < SPRING_COLS - 1) {
                Graphics::DrawLine(particles[index]->position.x, particles[index]->position.y,
                                   particles[index + 1]->position.x, particles[index + 1]->position.y, 0xFF0000FF);
            }
            if (i < SPRING_ROWS - 1) {
                Graphics::DrawLine(particles[index]->position.x, particles[index]->position.y,
                                   particles[index + SPRING_COLS]->position.x, particles[index + SPRING_COLS]->position.y, 0xFF0000FF);
            }
            
            if (i < SPRING_ROWS - 1 && j < SPRING_COLS - 1) {
                Graphics::DrawLine(particles[index]->position.x, particles[index]->position.y,
                                   particles[index + SPRING_COLS + 1]->position.x, particles[index + SPRING_COLS + 1]->position.y, 0xFF0000FF);
            }
            
            if (i < SPRING_ROWS - 1 && j > 0) {
                Graphics::DrawLine(particles[index]->position.x, particles[index]->position.y,
                                   particles[index + SPRING_COLS - 1]->position.x, particles[index + SPRING_COLS - 1]->position.y, 0xFF0000FF);
            }
        }
    }

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

void Application::CreateGridSoftBody(float x, float y, float mass) {
    for (int i = 0; i < SPRING_ROWS; i++) {
        for (int j = 0; j < SPRING_COLS; j++) {
            Particle *particle = new Particle(x + (i * 200), y + (j * 200), mass);
            particle->radius = 5.0f;
            //particle->color = 0xFF000000 + (0x00FFFFFF / SPRING_ROWS) * i;
            particles.push_back(particle);
        }
    }
}

void Application::ApplySpringForces() {
    // Apply spring forces
    for (int i = 0; i < SPRING_ROWS; i++) {
        for (int j = 0; j < SPRING_COLS; j++) {
            int index = i * SPRING_COLS + j;
            if (j < SPRING_COLS - 1) {
                Particle *particle1 = particles[index];
                Particle *particle2 = particles[index + 1];
                Vec2 force = Force::GenerateSpringForce(*particle1, *particle2, restLength, k);
                particle1->AddForce(force);
                particle2->AddForce(-force);
            }
            if (i < SPRING_ROWS - 1) {
                Particle *particle1 = particles[index];
                Particle *particle2 = particles[index + SPRING_COLS];
                Vec2 force = Force::GenerateSpringForce(*particle1, *particle2, restLength, k);
                particle1->AddForce(force);
                particle2->AddForce(-force);
            }
            if (i < SPRING_ROWS - 1 && j < SPRING_COLS - 1) {
                Particle *particle1 = particles[index];
                Particle *particle2 = particles[index + SPRING_COLS + 1];
                float diagonalRestLength = sqrt(restLength * restLength + restLength * restLength);
                Vec2 force = Force::GenerateSpringForce(*particle1, *particle2, diagonalRestLength, k);
                particle1->AddForce(force);
                particle2->AddForce(-force);
            }
            if (i < SPRING_ROWS - 1 && j > 0) {
                Particle *particle1 = particles[index];
                Particle *particle2 = particles[index + SPRING_COLS - 1];
                float diagonalRestLength = sqrt(restLength * restLength + restLength * restLength);
                Vec2 force = Force::GenerateSpringForce(*particle1, *particle2, diagonalRestLength, k);
                particle1->AddForce(force);
                particle2->AddForce(-force);
            }
        }
    }
}

