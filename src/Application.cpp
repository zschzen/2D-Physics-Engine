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
	
    // Create walls and floor
	Body* floor = new Body(BoxShape(Graphics::Width() - 50, 50), Graphics::Width() / 2, Graphics::Height() - 50, 0.0f);
	Body* leftWall = new Body(BoxShape(50, Graphics::Height() - 100), 50, Graphics::Height() / 2.0 - 25, 0.0f);
	Body* rightWall = new Body(BoxShape(50, Graphics::Height() - 100), Graphics::Width() - 50, Graphics::Height() / 2.0 - 25, 0.0f);
    
    floor->restitution = 0.2f;
    floor->SetTexture("./assets/metal.png");
    leftWall->restitution = 0.2f;
    leftWall->SetTexture("./assets/metal.png");
    rightWall->restitution = 0.2f;
    rightWall->SetTexture("./assets/metal.png");
	
	world->AddBody(floor);
	world->AddBody(leftWall);
	world->AddBody(rightWall);
    
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
	Body* centerObstacle = new Body(PolygonShape(pentVertices), Graphics::Width() / 2, Graphics::Height() / 2, 0.0f);
    centerObstacle->SetTexture("./assets/metal.png");
    centerObstacle->restitution = 0.1f;
    world->AddBody(centerObstacle);
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

		        if (event.button.button == SDL_BUTTON_LEFT) {
			        newBody = new Body(CircleShape(25), x, y, 5.0f);
					newBody->SetTexture("./assets/basketball.png");
		        }
				else if (event.button.button == SDL_BUTTON_RIGHT) {
			        newBody = new Body(BoxShape(50, 50), x, y, 5.0f);
					newBody->SetTexture("./assets/crate.png");
		        }
				else return;
				
				world->AddBody(newBody);
				break;
			default:
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
