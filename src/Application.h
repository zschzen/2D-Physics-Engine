#ifndef APPLICATION_H
#define APPLICATION_H

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define DELAY(timeToWait) emscripten_sleep(timeToWait)
#else
#define DELAY(timeToWait) SDL_Delay(timeToWait)
#endif


#include "./Graphics.h"
#include "./Entity.h"
#include "./QuadTree/QuadTree.h"

class Application
{
private:
	bool running = false;

	QuadTree::QuadTree<float, Entity> quadTree;
	QuadTree::Rectangle<float> boundary;
	QuadTree::Rectangle<float> queryRange{0, 0, 50, 50};

	std::vector<std::shared_ptr<Entity>> entities = std::vector<std::shared_ptr<Entity>>();
	std::vector<Contact> contacts = std::vector<Contact>();

	bool Debug = false;

public:
	Application() = default;
	~Application() = default;
	bool IsRunning();
	void Setup();
	void Input();
	void Update();
	void Render();
	void Destroy();
};

#endif
