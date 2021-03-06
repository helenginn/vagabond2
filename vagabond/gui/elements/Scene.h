// Copyright (C) 2021 Helen Ginn

#ifndef __practical__Scene__
#define __practical__Scene__

#include "SnowGL.h"
#include "SDL2/SDL.h"
#include "ButtonResponder.h"

class IndexResponder;
class Renderable;
class Button;
class Modal;
class TextButton;

class Scene : public SnowGL, public ButtonResponder
{
public:
	Scene(Scene *prev = nullptr);
	
	void setModal(Modal *modal)
	{
		_modal = modal;
	}
	
	void setBackScene(Scene *prev)
	{
		_previous = prev;
	}
	
	void removeModal();

	void preSetup();
	virtual void setup() = 0;
	virtual void refresh() {};
	void refreshNextRender()
	{
		_mustRefresh = true;
	}
	void setBackground();
	void addTitle(std::string);
	void setFutureTitle(std::string title)
	{
		_title = title;
	}

	void show();
	void queueToShow();
	void showBackButton();
	void hideBackButton();
	virtual void showSimple();
	virtual void render();

	virtual void mouseMoveEvent(double x, double y);
	virtual void mousePressEvent(double x, double y, SDL_MouseButtonEvent button);
	virtual void mouseReleaseEvent(double x, double y, SDL_MouseButtonEvent button);

	virtual void keyReleaseEvent(SDL_Keycode pressed) {};
	virtual void keyPressEvent(SDL_Keycode pressed);

	virtual void back(int num = 0);
	void buttonPressed(std::string tag, Button *button);
	
	virtual const bool hasIndexedObjects() const
	{
		return false;
	}
	
	void setInformation(std::string str);
protected:
	void reloadBackground();
	virtual std::vector<Renderable *> &pertinentObjects();
	void convertToGLCoords(double *x, double *y);
	virtual void checkIndexBuffer(double x, double y, 
	                              bool hover, bool arrow, bool left) {};
	void setCentrePixels(Renderable *r, int x, int y);
	void swapCursor(SDL_Cursor *newCursor);
	
	std::string _title;

	Renderable *_dragged = nullptr;
	Renderable *_background = nullptr;
	Modal *_modal = nullptr;
	Modal *_removeModal = nullptr;
	Scene *_previous = nullptr;
	Button *_back = nullptr;
	TextButton *_info = nullptr;
	
	SDL_Cursor *_cursor = nullptr;

	int _lastIdx;

	bool _moving = false;
	bool _mouseDown = false;
	bool _mustRefresh = false;
	
	static std::string _defaultBg;
};

#endif
