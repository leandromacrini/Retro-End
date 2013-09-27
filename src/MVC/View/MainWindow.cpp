#include "MainWindow.h"

#include "Image.h"
#include "Sprite.h"
#include "TextComponent.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

void goLeft(BaseView *);
void goRight(BaseView *);

void showLogo(BaseView * view)
{
	Image* logo = new Image();
	logo->setSize(RenderController::getInstance().getScreenWidth()/2, RenderController::getInstance().getScreenHeight() / 2);
	logo->setPosition(RenderController::getInstance().getScreenWidth()/4, RenderController::getInstance().getScreenHeight() / 4 +200);
	logo->setPath("data/logo white.png");
	logo->setOpacity(0);
	view->addChild(logo);

	Animation* a = new Animation();

	a->millisDuration = 2000;
	a->newOpacity = new unsigned char(255);
	a->moveOffset = new Eigen::Vector3f( 0, -200, 0 );
	a->endCallback = [view, logo] ()
	{
		Animation* a = new Animation();
		a->millisDuration = 2000;
		a->endCallback =  [view, logo] () { view->removeChild(logo); };
		view->animate(a);

		//showConsoles(view);
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

	//showConsoles();
}

MainWindow::~MainWindow()
{

}

bool MainWindow::input(InputConfig* config, Input input)
{

	return true;
}