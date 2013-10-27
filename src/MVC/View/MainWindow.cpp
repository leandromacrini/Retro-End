#include "MainWindow.h"

#include "Image.h"
#include "Label.h"
#include "Sprite.h"

#include "../Controller/AudioController.h"
#include "../Controller/RenderController.h"
#include "../Controller/InputController.h"

#include "ConsoleView.h"


using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

void goLeft(BaseView *);
void goRight(BaseView *);

void showConsoles(BaseView * view, ConsoleView* consoles)
{
	consoles->setSize((float)RenderController::getInstance().getScreenWidth(), (float)RenderController::getInstance().getScreenHeight());
	consoles->setPosition(0,0);
	view->addChild(consoles);
}

void showLogo(BaseView * view)
{
	ConsoleView* consoles = new ConsoleView(); //pre load images

	Image* logo = new Image();
	logo->setSize((float)RenderController::getInstance().getScreenWidth()/2, (float)RenderController::getInstance().getScreenHeight() / 2);
	logo->setPosition((float)RenderController::getInstance().getScreenWidth()/4, (float)RenderController::getInstance().getScreenHeight() / 4);
	logo->setPath("data/logo white.png");
	logo->setOpacity(0);
	view->addChild(logo);

	Animation* a = new Animation();

	a->millisDuration = 2000;
	a->endCallback = [view, logo, consoles] ()
	{
		shared_ptr<Sound> sound = AudioController::getInstance().createSound("data/sounds/splash.wav");
		sound->play();

		Animation* a = new Animation();

		a->millisDuration = 2000;
		a->newOpacity = new unsigned char(255);
		a->endCallback = [view, logo, consoles] ()
		{
			Animation* a = new Animation();
			a->millisDuration = 1000;
			a->newSize = new Eigen::Vector2f((float)RenderController::getInstance().getScreenWidth()/5, (float)RenderController::getInstance().getScreenHeight() / 5);
			a->moveOffset = new Eigen::Vector3f((float)RenderController::getInstance().getScreenWidth()/2, (float)-RenderController::getInstance().getScreenHeight() / 4, 0);
			a->endCallback =  [view, logo, consoles] ()
			{
				//view->removeChild(logo);
				showConsoles(view, consoles);
			};

			logo->animate(a);
		};

		logo->animate(a);
	};

	logo->animate(a);
}

void showInput(BaseView * view)
{
	Label* l = new Label();
	l->setText("Nuovo controller rilevato");
	l->setColor(0xFFFFFFFF);
	l->setPosition(
		RenderController::getInstance().getScreenWidth() - l->getFont()->sizeText("Nuovo controller rilevato").x(),
		(float) RenderController::getInstance().getScreenHeight());

	view->addChild(l);

	Animation* a = new Animation();

	a->millisDuration = 500;
	a->moveOffset = new Eigen::Vector3f(0, -100,0);
	a->endCallback = [view, l] ()
	{
		Animation* a = new Animation();

		a->millisDuration = 500;
		a->endCallback = [view, l] ()
		{
			Animation* a = new Animation();

			a->millisDuration = 500;
			a->moveOffset = new Eigen::Vector3f(0, 100,0);
			a->endCallback = [view, l] ()
			{
				view->removeChild(l);
			};

			l->animate(a);
		};

		l->animate(a);
	};

	l->animate(a);
}

MainWindow::MainWindow()
{
	this->setSize((float)RenderController::getInstance().getScreenWidth(),(float) RenderController::getInstance().getScreenHeight());

	Image* background = new Image();
	background->setSize((float)RenderController::getInstance().getScreenWidth(), (float)RenderController::getInstance().getScreenHeight());
	background->setPath("data/images/bg_pattern.png");
	background->setTiled(true);
	this->addChild(background);

	Sprite* yoshi = new Sprite();
	yoshi->setPosition((float)RenderController::getInstance().getScreenWidth()-64, 0);
	yoshi->setSize(64, 64);
	yoshi->setPath("data/images/yoshi.png");
	yoshi->setLoop(true);
	yoshi->start();
	this->addChild(yoshi);

	//showLogo(this);

	ConsoleView* consoles = new ConsoleView(); //pre load images
	showConsoles(this, consoles);

	//subscribe to events

	InputController::getInstance().onNewControllerDetected += [this](int e)
	{
		showInput(this);
	};

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