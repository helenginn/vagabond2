#ifndef __practical__Window__
#define __practical__Window__

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include "KeyResponder.h"
#include <set>
#include <stdlib.h>
#include <iostream>

#define BROWSER_WIDTH 1600.
#define BROWSER_HEIGHT 900.
#define ASPECT_RATIO (BROWSER_WIDTH / BROWSER_HEIGHT)

static bool _running = true;

class Scene;
class Renderable;

struct SDL_Renderer;
struct SDL_Window;

class Window
{
public:
	Window(int width = BROWSER_WIDTH, int height = BROWSER_HEIGHT);
	virtual ~Window();
	
	static SDL_Renderer *renderer()
	{
		return _renderer;
	}
	
	static void setKeyResponder(KeyResponder *responder)
	{
		_keyResponder = responder;
	}
	
	static void clearKeyResponder()
	{
		_keyResponder = NULL;
	}
	
	static void setCurrentScene(Scene *scene);
	static void setNextScene(Scene *scene)
	{
		_next = scene;
	}

	static void reloadScene(Scene *scene);
	
	static Scene *currentScene()
	{
		return _current;
	}
	
	static void setDelete(Scene *sc)
	{
		_deleteMutex.lock();
		_toDelete.insert(sc);
		_deleteMutex.unlock();
	}
	
	static void setDelete(Renderable *r)
	{
		_deleteMutex.lock();
		_deleteRenderables.insert(r);
		_deleteMutex.unlock();
	}

	void glSetup();
	virtual void setup(int argc, char **argv) = 0;
	static void render();
	virtual void mainThreadActivities() {};
	static bool tick();
	static void window_tick();
	
	static int height()
	{
		return _height;
	}
	
	static int width()
	{
		return _width;
	}
	
	static float aspect()
	{
		return (float)_rect.h / (float)_rect.w;
	}
	
	static bool hasContext()
	{
		return _context;
	}
	
	static Window *window()
	{
		return _myWindow;
	}
	static bool isNativeApp()
	{
#ifdef __EMSCRIPTEN__
		return false;
#endif
		char *native = getenv("MAC_NATIVE_APP");
		return (native != nullptr);
	}

protected:
	static Scene *_current;
	static Scene *_next;

	static Window *_myWindow;
private:
	void deleteQueued();

	static SDL_Renderer *_renderer;
	static SDL_Window *_window;
	static SDL_GLContext _context;
	static SDL_Rect _rect;

	static int _width;
	static int _height;

	static std::set<Scene *> _toDelete;
	static std::mutex _deleteMutex;
	static std::set<Renderable *> _deleteRenderables;
	
	static KeyResponder *_keyResponder;
};

#endif
