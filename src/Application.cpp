#include "Application.h"

#include "./Physics/CollisionDetection.h"
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

    // Create walls and floor
    Entity* floor = new Entity(BoxShape(Graphics::Width() - 50, 50), Graphics::Width() / 2, Graphics::Height() - 50, 0.0f);
    Entity* leftWall = new Entity(BoxShape(50, Graphics::Height() - 100), 50, Graphics::Height() / 2.0 - 25, 0.0f);
    Entity* rightWall = new Entity(BoxShape(50, Graphics::Height() - 100), Graphics::Width() - 50, Graphics::Height() / 2.0 - 25, 0.0f);
    
    floor->body->restitution = 0.2f;
    floor->SetTexture("./assets/metal.png");
    leftWall->body->restitution = 0.2f;
    leftWall->SetTexture("./assets/metal.png");
    rightWall->body->restitution = 0.2f;
    rightWall->SetTexture("./assets/metal.png");
    
    entities.push_back(floor);
    entities.push_back(leftWall);
    entities.push_back(rightWall);
    
    // TODO: Create a polygon generator function
    // Pentagram vertices local coordinates
    std::vector<Vec2> pentVertices = {
        Vec2(0, -100),
        Vec2(95, -31),
        Vec2(59, 81),
        Vec2(-59, 81),
        Vec2(-95, -31)
    };
    
    // centered right triangle vertices local coordinates
    std::vector<Vec2> rightTriangleVertices = {
        Vec2(0, -50),
        Vec2(50, 50),
        Vec2(-50, 50)
    };

    // Create obstacles
    Entity* centerObstacle = new Entity(BoxShape(100, 100), Graphics::Width() / 2, Graphics::Height() / 2, 0.0f);
    centerObstacle->SetTexture("./assets/metal.png");
    centerObstacle->body->restitution = 0.1f;
    entities.push_back(centerObstacle);
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
                break;
            case SDL_MOUSEBUTTONDOWN:
                int x, y;
                SDL_GetMouseState(&x, &y);
                Entity* newEntity;
                
                // left button
                if (event.button.button == SDL_BUTTON_LEFT) {
                    newEntity = new Entity(CircleShape(25), x, y, 1.0f);
                    newEntity->SetTexture("./assets/basketball.png");
                } else if (event.button.button == SDL_BUTTON_RIGHT) {
                    newEntity = new Entity(BoxShape(50, 50), x, y, 1.0f);
                    newEntity->SetTexture("./assets/crate.png");
                } else {
                    break;
                }
                entities.push_back(newEntity);
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
    if (timeToWait > 0) DELAY(timeToWait);

    // Calculate the deltatime in seconds
    float deltaTime = (SDL_GetTicks() - timePreviousFrame) / 1000.0f;
    if (deltaTime > MILLISECS_PER_FRAME) deltaTime = MILLISECS_PER_FRAME;

    // Set the time of the current frame to be used in the next one
    timePreviousFrame = SDL_GetTicks();

    // Clear contacts
    if (Debug) contacts.clear();

    // Add forces to the bodies
    for (auto& entity: entities) {
        // drag force
        if (!entity || entity->body->IsStatic()) continue;
        Vec2 weight = Vec2(0, GRAVITY * entity->body->mass * PIXELS_PER_METER);
        entity->body->AddForce(weight);
    }

    // Integrate the acceleration and velocity to estimate the new position
    for (auto& entity: entities) {
        entity->body->Update(deltaTime);
    }
    
    // Reset collision flags
    for (auto& entity: entities) {
        entity->body->isColliding = false;
    }
    
    // Check collisions
    for (int i = 0; i <= entities.size() - 1; ++i) {
        for (int j = i + 1; j < entities.size(); ++j) {
            Body *bodyA = entities[i]->body;
            Body *bodyB = entities[j]->body;
            
            Contact contact;
            if (!CollisionDetection::IsColliding(bodyA, bodyB, contact)) continue;

            // Resolve collision
            contact.ResolveCollision();

            if (Debug) contacts.push_back(contact);

            bodyA->isColliding = true;
            bodyB->isColliding = true;
        }
    }
    
    entities[3]->body->rotation += 0.25f * deltaTime;
}

///////////////////////////////////////////////////////////////////////////////
// Render function (called several times per second to draw objects)
///////////////////////////////////////////////////////////////////////////////
void Application::Render() {
    Graphics::ClearScreen(0xFF0F0721);

    for (const auto& entity : entities) {
        entity->Draw(Debug);
    }

    if (Debug) {
        for (const auto &contact: contacts) {
            Graphics::DrawFillCircle(contact.start.x, contact.start.y, 3, 0xFFFF00FF);
            Graphics::DrawFillCircle(contact.end.x, contact.end.y, 3, 0xFFFFFFFF);
            Graphics::DrawLine(contact.start.x, contact.start.y, contact.start.x + contact.normal.x * 15,
                               contact.start.y + contact.normal.y * 15, 0xFFFF00FF);
        }
    }
    
    Graphics::RenderFrame();
}

///////////////////////////////////////////////////////////////////////////////
// Destroy function to delete objects and close the window
///////////////////////////////////////////////////////////////////////////////
void Application::Destroy() {
    for (auto& entity: entities) {
        delete entity;
    }

    Graphics::CloseWindow();
}
