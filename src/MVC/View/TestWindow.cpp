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
	this->setSize((float)RenderController::getInstance().getScreenWidth(),(float) RenderController::getInstance().getScreenHeight());
	
	Image* background = new Image();
	background->setSize((float)RenderController::getInstance().getScreenWidth(), (float)RenderController::getInstance().getScreenHeight());
	background->setPath("data/images/sfondo2.jpg");
	this->addChild(background);

	Label* title = new Label();
	title->setText("TEST WINDOW");
	title->setPosition(0,0);
	title->setColor(0xFFFFFFFF);

	this->addChild(title);

	myList = new ListView();

	myList->setPosition(100, 50);
	myList->setSize(500, 425);
	myList->HorizontalTextAlign = TextAlign::Center;
	myList->SelectedRowBackgroundColor = 0xFFFFFFFF;

	for(int i = 0 ; i < 100 ; i++)
	{
		myList->addRow("Titolo del gioco " + to_string(i));
	}
	
	this->addChild(myList);

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
