
#include "TestWindow.h"

#include "ConsoleView.h"
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

	//mConsoleView = new ConsoleView(); //pre load images

	Image* logo = new Image();
	logo->setSize((float)RenderController::getInstance().getScreenWidth()/2, (float)RenderController::getInstance().getScreenHeight() / 2);
	logo->setPosition((float)RenderController::getInstance().getScreenWidth()/4, (float)RenderController::getInstance().getScreenHeight() / 4);
	logo->setPath("data/logo black.png");
	logo->setOpacity(0);
	addChild(logo);

	Animation* a = new Animation();

	a->millisDuration = 2000;
	a->endCallback = [this, logo] ()
	{
		Animation* a = new Animation();

		a->millisDuration = 2000;
		a->newOpacity = new unsigned char(255);
		a->endCallback = [this, logo] ()
		{
			Animation* a = new Animation();
			a->millisDuration = 1000;
			a->newSize = new Eigen::Vector2f((float)RenderController::getInstance().getScreenWidth()/5, (float)RenderController::getInstance().getScreenHeight() / 5);
			a->moveOffset = new Eigen::Vector3f((float)RenderController::getInstance().getScreenWidth()/2, (float)-RenderController::getInstance().getScreenHeight() / 4, 0);
			a->endCallback =  [this] ()
			{
				//Show Consoles
				//mConsoleView->setSize((float)RenderController::getInstance().getScreenWidth(), (float)RenderController::getInstance().getScreenHeight());
				//mConsoleView->setPosition(0,0);
				//addChild(mConsoleView);
			};

			logo->animate(a);
		};

		logo->animate(a);
	};

	logo->animate(a);

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
