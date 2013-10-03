#pragma once

#include "TestWindow.h"

#include "Label.h"
#include "Image.h"
#include "../Model/Font.h"
#include "../Controller/ResourceController.h"
#include "../Controller/RenderController.h"

using namespace RetroEnd::View;
using namespace RetroEnd::Model;
using namespace RetroEnd::Controller;

TestWindow::TestWindow()
{
	this->setSize(RenderController::getInstance().getScreenWidth(), RenderController::getInstance().getScreenHeight());
	
	Image* background = new Image();
	background->setSize((float)RenderController::getInstance().getScreenWidth(), (float)RenderController::getInstance().getScreenHeight());
	background->setPath("data/sfondo2.jpg");
	this->addChild(background);

	BaseView* view1 = new BaseView();
	view1->setPosition(0,0);
	view1->setSize(100,100);
	view1->setBackgroundColor(0xFF0000FF);
	this->addChild(view1);

	Label* title = new Label();
	title->setText(" TEST WINDOW");
	title->setPosition(0,0);
	title->setColor(0xFFFFFFFF);

	view1->addChild(title);

}

bool TestWindow::input(Model::InputConfig* config, Model::Input input)
{
	if(mChildren.size() > 0)
	{
		mChildren.at(mChildren.size() - 1)->input(config, input);
	}
	return true;
}
