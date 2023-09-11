#include "Application.h"

#include "./Physics/Constants.h"


bool Application::IsRunning() {
    return running;
}

///////////////////////////////////////////////////////////////////////////////
// Setup function (executed once in the beginning of the simulation)
///////////////////////////////////////////////////////////////////////////////
void Application::Setup() {
    running = Graphics::OpenWindow();

	world = new World(-9.8f);
	
    // Add two bodies
    Body* bodyA = new Body(CircleShape(30.0f), Graphics::Width() / 2.0f, Graphics::Height() / 2.0f, 0.0f);
    Body* bodyB = new Body(CircleShape(20.0f), bodyA->position.x - 100.0f, bodyA->position.y, 1.0f);
    world->AddBody(bodyA);
    world->AddBody(bodyB);
    
    // Add a joint constraint between the two bodies
    JointConstraint* joint = new JointConstraint(bodyA, bodyB, bodyA->position);
    world->AddConstraint(joint);
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
			default: break;
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

	// Update the world
    world->Update(deltaTime);
}

///////////////////////////////////////////////////////////////////////////////
// Render function (called several times per second to draw objects)
///////////////////////////////////////////////////////////////////////////////
void Application::Render() {
    Graphics::ClearScreen(0xFF0F0721);

    const std::vector<Body*> bodies = world->GetBodies();
	for (const auto& body: bodies) {
		Uint32 color = body->isColliding ? 0xFF0000FF : 0xFFFFFFFF;

		switch (body->shape->GetType()) {
			case ShapeType::CIRCLE: {
				CircleShape *circle = dynamic_cast<CircleShape *>(body->shape);
				if (Debug || !body->texture)
					Graphics::DrawCircle(body->position.x, body->position.y, circle->radius, body->rotation, color);
				else
					Graphics::DrawTexture(body->position.x, body->position.y, circle->radius * 2, circle->radius * 2,
					                      body->rotation, body->texture);
				break;
			}
			case ShapeType::BOX: {
				BoxShape *box = dynamic_cast<BoxShape *>(body->shape);
				if (Debug || !body->texture)
					Graphics::DrawPolygon(body->position.x, body->position.y, box->worldVertices, color);
				else
					Graphics::DrawTexture(body->position.x, body->position.y, box->width, box->height, body->rotation,
					                      body->texture);
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
    
    Graphics::RenderFrame();
}

///////////////////////////////////////////////////////////////////////////////
// Destroy function to delete objects and close the window
///////////////////////////////////////////////////////////////////////////////
void Application::Destroy() {
	delete world;
    Graphics::CloseWindow();
}
