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

    // Add several bodies
    constexpr int NUM_BODIES = 8;
    for (int i = 0; i < NUM_BODIES; i++) {
        float mass = (i == 0) ? 0.0f : 1.0f;
        Body* body = new Body(BoxShape(30, 30), (Graphics::Width() / 2.0) - (i * 40), 100, mass);
        body->SetTexture("./assets/crate.png");
        world->AddBody(body);
    }

    // Add joints to connect them (distance constraints)
    for (int i = 0; i < NUM_BODIES - 1; i++) {
        Body* a = world->GetBodies()[i];
        Body* b = world->GetBodies()[i + 1];
        JointConstraint* joint = new JointConstraint(a, b, a->position);
        world->AddConstraint(joint);
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
                if (event.key.keysym.sym == SDLK_d)
                    Debug = !Debug;
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    int x, y;
                    SDL_GetMouseState(&x, &y);
                    Body* ball = new Body(CircleShape(30), x, y, 1.0);
                    ball->SetTexture("./assets/basketball.png");
                    ball->restitution = 0.7;
                    world->AddBody(ball);
                }
                if (event.button.button == SDL_BUTTON_RIGHT) {
                    int x, y;
                    SDL_GetMouseState(&x, &y);
                    Body* box = new Body(BoxShape(60, 60), x, y, 1.0);
                    box->SetTexture("./assets/crate.png");
                    box->restitution = 0.2;
                    world->AddBody(box);
                }
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

    // Draw joint constraints
    const std::vector<Constraint*> constraints = world->GetConstraints();
    for (const auto& constraint: constraints) {
            JointConstraint* joint = dynamic_cast<JointConstraint*>(constraint);
            const Vec2 pa = joint->a->GetWorldPoint(joint->aPoint);
            const Vec2 pb = joint->b->GetWorldPoint(joint->aPoint);
            Graphics::DrawLine(pa.x, pa.y, pb.x, pb.y, 0xFF00FF00);
    }
    
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
