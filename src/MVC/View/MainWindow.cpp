#include "MainWindow.h"

#include "Image.h"
#include "Sprite.h"

#include "../Controller/AudioController.h"
#include "../Controller/RenderController.h"

#include "ConsoleView.h"


using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

void goLeft(BaseView *);
void goRight(BaseView *);

void showConsoles(BaseView * view)
{
	ConsoleView* consoles = new ConsoleView();
	consoles->setSize((float)RenderController::getInstance().getScreenWidth(), (float)RenderController::getInstance().getScreenHeight());
	consoles->setPosition(0,0);
	view->addChild(consoles);
}

void showLogo(BaseView * view)
{
	Image* logo = new Image();
	logo->setSize((float)RenderController::getInstance().getScreenWidth()/2, (float)RenderController::getInstance().getScreenHeight() / 2);
	logo->setPosition((float)RenderController::getInstance().getScreenWidth()/4, (float)RenderController::getInstance().getScreenHeight() / 4);
	logo->setPath("data/logo white.png");
	logo->setOpacity(0);
	view->addChild(logo);

	Animation* a = new Animation();

	a->millisDuration = 2000;
	a->endCallback = [view, logo] ()
	{
		shared_ptr<Sound> sound = AudioController::getInstance().createSound("data/sounds/splash.wav");
		sound->play();

		Animation* a = new Animation();

		a->millisDuration = 2000;
		a->newOpacity = new unsigned char(255);
		a->endCallback = [view, logo] ()
		{
			

			Animation* a = new Animation();
			a->millisDuration = 1000;
			a->newSize = new Eigen::Vector2f((float)RenderController::getInstance().getScreenWidth()/5, (float)RenderController::getInstance().getScreenHeight() / 5);
			a->moveOffset = new Eigen::Vector3f((float)RenderController::getInstance().getScreenWidth()/2, (float)-RenderController::getInstance().getScreenHeight() / 4, 0);
			a->endCallback =  [view, logo] ()
			{
				//view->removeChild(logo);
				showConsoles(view);
			};

			logo->animate(a);
		};

		logo->animate(a);
	};

	logo->animate(a);
}

MainWindow::MainWindow()
{
	Image* background = new Image();
	background->setSize((float)RenderController::getInstance().getScreenWidth(), (float)RenderController::getInstance().getScreenHeight());
	background->setPath("data/sfondo2.jpg");
	this->addChild(background);

	Sprite* yoshi = new Sprite();
	yoshi->setPosition((float)RenderController::getInstance().getScreenWidth()-64, (float)RenderController::getInstance().getScreenHeight()-64);
	yoshi->setSize(32, 32);
	yoshi->setPath("data/yoshi.png");
	yoshi->setLoop(true);
	yoshi->start();
	this->addChild(yoshi);

	showLogo(this);

	//showConsoles(this);
}

MainWindow::~MainWindow()
{

}

bool MainWindow::input(InputConfig* config, Input input)
{
	if(mChildren.size() > 0)
	{
		mChildren.at(mChildren.size() - 1)->input(config, input);
	}
	return true;
}