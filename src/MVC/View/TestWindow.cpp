#pragma once

#include "TestWindow.h"

#include "Label.h"
#include "Image.h"
#include "ListView.h"
#include "../Model/Font.h"
#include "../Controller/ResourceController.h"
#include "../Controller/RenderController.h"

using namespace RetroEnd::View;
using namespace RetroEnd::Model;
using namespace RetroEnd::Controller;

ListView* myList;

TestWindow::TestWindow()
{	
	float H = (float)RenderController::getInstance().getScreenHeight();
	float W = (float)RenderController::getInstance().getScreenWidth();

	this->setSize( W, H );

	Image* background = new Image();
	background->setSize( W, H );
	background->setPath("data/images/bg_bn.png");
	background->setTiled(true);
	addChild(background);

	Label* title = new Label();
	title->setText("TEST WINDOW");
	title->setPosition(550,100);
	title->setColor(0x000000FF);

	this->addChild(title);

}

bool TestWindow::input(Model::InputConfig* config, Model::Input input)
{
	//TODO input from settings
	if(input.id == SDLK_DOWN && input.value != 0 )
	{
		myList->setSelectedIndex( myList->getSelectedIndex() + 1 );
		return true;
	}

	if(input.id == SDLK_UP && input.value != 0)
	{
		myList->setSelectedIndex( myList->getSelectedIndex() - 1 );
		return true;
	}

	if(mChildren.size() > 0)
	{
		mChildren.at(mChildren.size() - 1)->input(config, input);
	}

	return true;
}
