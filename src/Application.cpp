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
    Body* floorBox = new Body(BoxShape(Graphics::Width() - 50, 50), Graphics::Width() / 2, Graphics::Height() - 50, 0.0f);
    Body* leftWallBox = new Body(BoxShape(50, Graphics::Height() - 100), 50, Graphics::Height() / 2.0 - 25, 0.0);
    Body* rightWallBox = new Body(BoxShape(50, Graphics::Height() - 100), Graphics::Width() - 50, Graphics::Height() / 2.0 - 25, 0.0);
    floorBox->restitution = 0.2f;
    leftWallBox->restitution = 0.2f;
    rightWallBox->restitution = 0.2f;
    bodies.push_back(floorBox);
    bodies.push_back(leftWallBox);
    bodies.push_back(rightWallBox);
    
    // TODO: Create a polygon generator function
    // Pentagram vertices local coordinates
    std::vector<Vec2> pentVertices = {
        Vec2(0, -100),
        Vec2(95, -31),
        Vec2(59, 81),
        Vec2(-59, 81),
        Vec2(-95, -31)
    };
    
    // Right triangle vertices local coordinates
    std::vector<Vec2> rightTriangleVertices = {
        Vec2(0, 0),
        Vec2(100, 0),
        Vec2(0, 100)
    };

    // Create obstacles
    Body* pBody = new Body(PolygonShape(pentVertices), Graphics::Width() / 2, Graphics::Height() / 2, 0.0f);
    pBody->restitution = 0.5f;
    bodies.push_back(pBody);
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
                Body* newBody;
                
                // left button
                if (event.button.button == SDL_BUTTON_LEFT) {
                    newBody = new Body(CircleShape(25), x, y, 1.0f);
                } else if (event.button.button == SDL_BUTTON_RIGHT) {
                    newBody = new Body(BoxShape(50, 50), x, y, 1.0f);
                } else {
                    break;
                }
                bodies.push_back(newBody);
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
    //contacts.clear();

    // Add forces to the bodies
    for (auto& body: bodies) {
        // drag force
        if (!body || body->IsStatic()) continue;
        Vec2 weight = Vec2(0, GRAVITY * body->mass * PIXELS_PER_METER);
        body->AddForce(weight);
    }

    // Integrate the acceleration and velocity to estimate the new position
    for (auto& body: bodies) {
        body->Update(deltaTime);
    }
    
    // Reset collision flags
    for (auto& body: bodies) {
        body->isColliding = false;
    }
    
    // Check collisions
    for (int i = 0; i <= bodies.size() - 1; ++i) {
        for (int j = i + 1; j < bodies.size(); ++j) {
            Contact contact;
            if (!CollisionDetection::IsColliding(bodies[i], bodies[j], contact)) continue;

            // Resolve collision
            contact.ResolveCollision();

            //contacts.push_back(contact);
            
            bodies[i]->isColliding = true;
            bodies[j]->isColliding = true;
        }
    }

    // if anybody is off-screen, delete it safely
    for (auto it = bodies.begin(); it != bodies.end();) {
        if ((*it)->position.x < 0 || (*it)->position.x > Graphics::windowWidth ||
            (*it)->position.y < 0 || (*it)->position.y > Graphics::windowHeight) {
            delete *it;
            it = bodies.erase(it);
        } else {
            ++it;
        }
    }
    
    bodies[3]->rotation += 0.25f * deltaTime;
}

///////////////////////////////////////////////////////////////////////////////
// Render function (called several times per second to draw objects)
///////////////////////////////////////////////////////////////////////////////
void Application::Render() {
    Graphics::ClearScreen(0xFF0F0721);

    if (selectedBody) {
        Graphics::DrawLine(selectedBody->position.x, selectedBody->position.y, mouseCursor.x, mouseCursor.y, 0xFF0000FF);
    }
    
    for (const auto &body: bodies) {
        Uint32 color = body->isColliding ? 0xFF0000FF : 0xFFFFFFFF;
        
        switch (body->shape->GetType()) {
            case ShapeType::CIRCLE: {
                CircleShape *circle = dynamic_cast<CircleShape *>(body->shape);
                Graphics::DrawCircle(body->position.x, body->position.y, circle->radius, body->rotation, color);
                break;
            }
            case ShapeType::BOX: {
                BoxShape *box = dynamic_cast<BoxShape *>(body->shape);
                Graphics::DrawPolygon(body->position.x, body->position.y, box->worldVertices, color);
                break;
            }
            case ShapeType::POLYGON: {
                PolygonShape *polygon = dynamic_cast<PolygonShape *>(body->shape);
                Graphics::DrawPolygon(body->position.x, body->position.y, polygon->worldVertices, color);
                break;
            }
            default:
                break;
        }
    }

    /*
    for (const auto& contact: contacts) {
        Graphics::DrawFillCircle(contact.start.x, contact.start.y, 3, 0xFFFF00FF);
        Graphics::DrawFillCircle(contact.end.x, contact.end.y, 3, 0xFFFFFFFF);
        Graphics::DrawLine(contact.start.x, contact.start.y, contact.start.x + contact.normal.x * 15, contact.start.y + contact.normal.y * 15, 0xFFFF00FF);
    }
    */
    
    Graphics::RenderFrame();
}

///////////////////////////////////////////////////////////////////////////////
// Destroy function to delete objects and close the window
///////////////////////////////////////////////////////////////////////////////
void Application::Destroy() {
    for (const auto &body: bodies) {
        delete body;
    }

    Graphics::CloseWindow();
}

Body *Application::FindClosestBody(const Vec2 &position)
{
    Body *closestbody = nullptr;
    float closestDistance = 0.0f;

    for (const auto &body: bodies) {
        float distance = (body->position - position).Magnitude();
        if (closestbody && distance >= closestDistance) continue;

        closestbody = body;
        closestDistance = distance;
    }

    return closestbody;
}