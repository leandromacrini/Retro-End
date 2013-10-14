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

TestWindow::TestWindow()
{
	this->setSize((float)RenderController::getInstance().getScreenWidth(),(float) RenderController::getInstance().getScreenHeight());
	
	Image* background = new Image();
	background->setSize((float)RenderController::getInstance().getScreenWidth(), (float)RenderController::getInstance().getScreenHeight());
	background->setPath("data/sfondo2.jpg");
	this->addChild(background);

	Label* title = new Label();
	title->setText("TEST WINDOW");
	title->setPosition(0,0);
	title->setColor(0xFFFFFFFF);

	this->addChild(title);

	ListView* list = new ListView();

	list->setPosition(100, 50);
	list->setSize(500, 500);
	list->setBackgroundColor(0x00FF00FF);

	list->addRow("PROVA 1");
	list->addRow("PROVA 2");
	list->addRow("PROVA 3");
	list->addRow("PROVA 4");
	list->addRow("PROVA 5");
	list->addRow("PROVA 6");
	list->addRow("PROVA 7");
	list->addRow("PROVA 8");
	list->addRow("PROVA 9");
	list->addRow("PROVA 10");
	list->addRow("PROVA 11");
	list->addRow("PROVA 12");
	list->addRow("PROVA 13");
	this->addChild(list);

}

bool TestWindow::input(Model::InputConfig* config, Model::Input input)
{
	if(mChildren.size() > 0)
	{
		mChildren.at(mChildren.size() - 1)->input(config, input);
	}
	return true;
}
