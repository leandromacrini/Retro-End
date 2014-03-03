
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
}

bool TestWindow::input(Model::Input input)
{
	if(input.RawData.ValueID == SDLK_p && input.Value != 0 )
	{
		RenderController::getInstance().pushPopupMessage("Miao testo lungo lungo lungo lungo lungo lungo lungo lungo lungo!", PopupMessageIcon::Info);
		return true;
	}

	//set input only to the last view added
	if(mChildren.size() > 0)
	{
		mChildren.at(mChildren.size() - 1)->input(input);
	}
	return true;
}
