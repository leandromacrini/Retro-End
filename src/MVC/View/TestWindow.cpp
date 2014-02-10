
#include "TestWindow.h"

#include "ConsoleView.h"
#include "Label.h"
#include "Image.h"
#include "ListView.h"
#include "../Model/Font.h"
#include "../Controller/AudioController.h"
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
	title->setPosition(550,10);
	title->setColor(0x000000FF);

	this->addChild(title);

	//Show Consoles
	mConsoleView = new ConsoleView(); //pre load images
	mConsoleView->setSize((float)RenderController::getInstance().getScreenWidth(), (float)RenderController::getInstance().getScreenHeight());
	mConsoleView->setPosition(0,0);
	addChild(mConsoleView);
}

bool TestWindow::input(Model::InputConfig* config, Model::Input input)
{
	if(input.id == SDLK_p && input.value != 0 )
	{
		RenderController::getInstance().pushPopupMessage("Miao testo lungo lungo lungo lungo lungo lungo lungo lungo lungo!", PopupMessageIcon::Info);
		return true;
	}

	//set input only to the last view added
	if(mChildren.size() > 0)
	{
		mChildren.at(mChildren.size() - 1)->input(config, input);
	}
	return true;
}
