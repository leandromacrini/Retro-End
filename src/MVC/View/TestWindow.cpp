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
	Image* background = new Image();
	background->setSize((float)RenderController::getInstance().getScreenWidth(), (float)RenderController::getInstance().getScreenHeight());
	background->setPath("data/sfondo2.jpg");
	this->addChild(background);

	Label* title = new Label();
	title->setText(" TEST WINDOW");
	title->setPosition(0,0);
	title->setColor(0xFFFFFFFF);

	this->addChild(title);
}

bool TestWindow::input(Model::InputConfig* config, Model::Input input)
{
	if(mChildren.size() > 0)
	{
		mChildren.at(mChildren.size() - 1)->input(config, input);
	}
	return true;
}
