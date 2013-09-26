#include "MainWindow.h"

#include "Image.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

void goLeft(BaseView *);
void goRight(BaseView *);

void goLeft(BaseView * view)
{
	Animation* a = new Animation();

	a->millisDuration = 500;
	a->moveOffset = new Eigen::Vector3f(-100,0,0);
	a->endCallback = [view] () { goRight( view ); };

	view->animate(a);
}

void goRight(BaseView * view)
{
	Animation* a = new Animation();

	a->millisDuration = 500;
	a->moveOffset = new Eigen::Vector3f(100,0,0);
	a->endCallback = [view] () { goLeft( view ); };

	view->animate(a);
}

MainWindow::MainWindow()
{
	BaseView * view = new BaseView();

	view->setPosition(100,100);
	view->setSize(400,400);
	view->setBackgroundColor(0x0000FFFF);

	Image * child = new Image();
	child->setPosition(0, 50);
	child->setSize(100, 100);
	child->setPath("face.png");
	
	view->addChild(child);

	this->addChild(	view );

	goLeft(view);
}

MainWindow::~MainWindow()
{

}

bool MainWindow::isRunning()
{

	return true;
}

bool MainWindow::input(InputConfig* config, Input input)
{

	return true;
}