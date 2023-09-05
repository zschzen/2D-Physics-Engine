#include "Application.h"

#include "./Physics/CollisionDetection.h"
#include "./Physics/Constants.h"
#include "./Physics/Force.h"


bool Application::IsRunning()
{
	return running;
}

///////////////////////////////////////////////////////////////////////////////
// Setup function (executed once in the beginning of the simulation)
///////////////////////////////////////////////////////////////////////////////
void Application::Setup()
{
	running = Graphics::OpenWindow();

	boundary = QuadTree::Rectangle<float>(0, 0, Graphics::Width(), Graphics::Height());
	quadTree = QuadTree::QuadTree<float, Entity>(boundary);

	// Create walls and floor
	auto floor = std::make_shared<Entity>(BoxShape(Graphics::Width() - 50, 50), Graphics::Width() / 2,
	                                      Graphics::Height() - 50, 0.0f);
	auto leftWall = std::make_shared<Entity>(BoxShape(50, Graphics::Height() - 100), 50, Graphics::Height() / 2.0 - 25,
	                                         0.0f);
	auto rightWall = std::make_shared<Entity>(BoxShape(50, Graphics::Height() - 100), Graphics::Width() - 50,
	                                          Graphics::Height() / 2.0 - 25, 0.0f);

	floor->body->restitution = 0.2f;
	floor->SetTexture("./assets/metal.png");
	leftWall->body->restitution = 0.2f;
	leftWall->SetTexture("./assets/metal.png");
	rightWall->body->restitution = 0.2f;
	rightWall->SetTexture("./assets/metal.png");

	entities.push_back(floor);
	quadTree.insert(QuadTree::Point<float, Entity>(floor->body->position.x, floor->body->position.y, floor));
	entities.push_back(leftWall);
	quadTree.insert(QuadTree::Point<float, Entity>(leftWall->body->position.x, leftWall->body->position.y, leftWall));
	entities.push_back(rightWall);
	quadTree.insert(
			QuadTree::Point<float, Entity>(rightWall->body->position.x, rightWall->body->position.y, rightWall));

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
	auto centerObstacle = std::make_shared<Entity>(BoxShape(100, 100), Graphics::Width() / 2, Graphics::Height() / 2,
	                                               0.0f);
	centerObstacle->SetTexture("./assets/metal.png");
	centerObstacle->body->restitution = 0.1f;
	entities.push_back(centerObstacle);
	quadTree.insert(QuadTree::Point<float, Entity>(centerObstacle->body->position.x, centerObstacle->body->position.y,
	                                               centerObstacle));
}

///////////////////////////////////////////////////////////////////////////////
// Input processing
///////////////////////////////////////////////////////////////////////////////
void Application::Input()
{
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
			case SDL_MOUSEMOTION:
				int x, y;
				SDL_GetMouseState(&x, &y);
				queryRange.position = QuadTree::Vector2 < float > (x, y);
				break;
			case SDL_MOUSEWHEEL:
				if (event.wheel.y < 0) {
					if (queryRange.size.x > Graphics::Width() / 4) break;
					if (queryRange.size.y > Graphics::Height() / 4) break;
					queryRange.size *= 1.5f;
				} else if (event.wheel.y > 0) {
					if (queryRange.size.x < 30) break;
					if (queryRange.size.y < 30) break;
					queryRange.size *= 0.5f;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				//int x, y;
				SDL_GetMouseState(&x, &y);
				std::shared_ptr<Entity> newEntity;

				// left button
				if (event.button.button == SDL_BUTTON_LEFT) {
					newEntity = std::make_shared<Entity>(CircleShape(25), x, y, 1.0f);
					newEntity->SetTexture("./assets/basketball.png");
				} else if (event.button.button == SDL_BUTTON_RIGHT) {
					newEntity = std::make_shared<Entity>(BoxShape(50, 50), x, y, 1.0f);
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
void Application::Update()
{
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
	for (auto &entity: entities) {
		// drag force
		if (!entity || entity->body->IsStatic()) continue;
		Vec2 weight = Vec2(0, GRAVITY * entity->body->mass * PIXELS_PER_METER);
		entity->body->AddForce(weight);
	}

	// Clear the quadtree
	quadTree = QuadTree::QuadTree<float, Entity>(boundary);

	// Integrate the acceleration and velocity to estimate the new position
	for (auto &entity: entities) {
		entity->body->Update(deltaTime);
		entity->body->isColliding = false;

		// Insert the entity into the quadtree
		QuadTree::Point<float, Entity> point(entity->body->position.x, entity->body->position.y, entity);
		quadTree.insert(point);
	}

	// Check collisions
	if (Debug) contacts.clear();
	for (auto &entity: entities) {
		Body *body = entity->body;
		ShapeType type = body->shape->GetType();

		std::vector<QuadTree::Point<float, Entity>> nearbyPoints;
		QuadTree::Rectangle<float> range(body->position.x, body->position.y, 0, 0);

		switch (type) {
			case ShapeType::CIRCLE: {
				CircleShape *circle = dynamic_cast<CircleShape *>(body->shape);
				range.size = QuadTree::Vector2 < float > (circle->radius * 2, circle->radius * 2);
				break;
			}
			case ShapeType::BOX: {
				BoxShape *box = dynamic_cast<BoxShape *>(body->shape);
				range.size = QuadTree::Vector2 < float > (box->width * 2, box->height * 2);
				break;
			}
		}

		quadTree.query(range, nearbyPoints);

		for (auto &nearbyPoint: nearbyPoints) {
			if (nearbyPoint.data == entity) continue;

			Contact contact;
			if (!CollisionDetection::IsColliding(body, nearbyPoint.data->body, contact)) continue;

			if (Debug) contacts.push_back(contact);

			contact.ResolveCollision();

			body->isColliding = true;
			nearbyPoint.data->body->isColliding = true;
		}
	}

	// If any entity goes out of bounds, delete it
	for (auto it = entities.begin(); it != entities.end();) {
		if ((*it)->body->position.y > Graphics::Height() + 100 ||
		    (*it)->body->position.x < -100 ||
		    (*it)->body->position.x > Graphics::Width() + 100) {
			it = entities.erase(it);
		} else {
			++it;
		}
	}

	entities[3]->body->rotation += 0.25f * deltaTime;
}

///////////////////////////////////////////////////////////////////////////////
// Render function (called several times per second to draw objects)
///////////////////////////////////////////////////////////////////////////////
void Application::Render()
{
	Graphics::ClearScreen(0xFF0F0721);

	if (Debug) {
		// Draw all quadtree boundries in white
		std::vector<QuadTree::Rectangle<float> *> allBoundries = quadTree.getAllBoundaries();
		for (const auto &boundry: allBoundries) {
			Graphics::DrawRect(boundry->position.x, boundry->position.y,
			                   boundry->size.x * 2, boundry->size.y * 2, 0xFFFFFFFF);
		}
	}

	for (const auto &entity: entities) {
		entity->Draw(Debug);
	}

	if (Debug) {
		for (const auto &contact: contacts) {
			Graphics::DrawFillCircle(contact.start.x, contact.start.y, 3, 0xFFFF00FF);
			Graphics::DrawFillCircle(contact.end.x, contact.end.y, 3, 0xFFFFFFFF);
			Graphics::DrawLine(contact.start.x, contact.start.y, contact.start.x + contact.normal.x * 15,
			                   contact.start.y + contact.normal.y * 15, 0xFFFF00FF);
		}

		std::vector<QuadTree::Point<float, Entity>> foundPoints;
		quadTree.query(queryRange, foundPoints);

		for (const auto &point: foundPoints) {
			Body *body = point.data->body;
			ShapeType type = body->shape->GetType();
			float w, h;

			switch (type) {
				case ShapeType::CIRCLE: {
					CircleShape *circle = dynamic_cast<CircleShape *>(body->shape);
					h = w = circle->radius * 2;
					break;
				}
				case ShapeType::BOX: {
					BoxShape *box = dynamic_cast<BoxShape *>(body->shape);
					w = box->width * 2;
					h = box->height * 2;
					break;
				}
			}

			// Draw interaction range
			Graphics::DrawRect(body->position.x, body->position.y, w, h, 0xFFFF79C6);
		}

		// Draw query range in green
		Graphics::DrawRect(queryRange.position.x, queryRange.position.y, queryRange.size.x * 2,
		                   queryRange.size.y * 2, 0xFF50FA7B);
	}

	Graphics::RenderFrame();
}

///////////////////////////////////////////////////////////////////////////////
// Destroy function to delete objects and close the window
///////////////////////////////////////////////////////////////////////////////
void Application::Destroy()
{
	for (auto &entity: entities) {
		entity = nullptr;
	}

	Graphics::CloseWindow();
}


