
#include "TestWindow.h"

#include "../Model/Font.h"

#include "Button.h"
#include "ConsoleView.h"
#include "Image.h"
#include "Label.h"
#include "ListView.h"
#include "MultiSelect.h"
#include "ProgressBar.h"
#include "Switch.h"
#include "DatePickerDialog.h"
#include "TextInputDialog.h"

#include "../Controller/AudioController.h"
#include "../Controller/InputController.h"
#include "../Controller/ResourceController.h"
#include "../Controller/RenderController.h"
#include "../Controller/SocialController.h"

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
	addChild(title);

	for(int i = 0; i < 5; i++)
	{
		Label* focus1 = new Label();
		focus1->setText("Label " + to_string(i));
		focus1->setPosition(100, 100 + (i*40));
		focus1->FocusedBackgroundColor = 0x00FF00FF;
		focus1->Focusable = true;
		focus1->FocusIndex = i;
		addChild(focus1);
	}

	Label* focus1 = new Label(true);
	focus1->setText("Text");
	focus1->setPosition(100, 100 + (6*40));
	focus1->FocusedBackgroundColor = 0x00FF00FF;
	focus1->Focusable = true;
	focus1->FocusIndex = 6;
	focus1->onLabelPressed += [this, focus1](int)
	{
		TextInputDialog* dialog = new TextInputDialog("Testo da inserire", focus1->getText(), [focus1](string value)
		{
			focus1->setText(value);
		}, true);
	};
	addChild(focus1);

	vector<string>* values = new vector<string>;

	values->push_back("DirectX");
	values->push_back("OpenGL");
	values->push_back("SDL");
	values->push_back("SDL2");
	values->push_back("TESTO LUNGO LUNGO LUNGO LUNGO LUNGO LUNGO LUNGO LUNGO LUNGO LUNGO LUNGO LUNGO LUNGO");

	MultiSelect* ms = new MultiSelect("Choose the value for the Render Type", "Render type", values, 0);
	ms->setPosition(100, 480);
	ms->setSize(150, 50);
	ms->FocusedBackgroundColor = 0x00FF00FF;
	ms->Focusable = true;
	ms->FocusIndex = 8;
	ms->onSelectedItemChanged += [](int index)
	{
		RenderController::getInstance().pushPopupMessage("Value is now: " + to_string(index), PopupMessageIcon::Info);
	};
	addChild(ms);

	Switch* sw1 = new Switch(true);
	sw1->setPosition(100, 540);
	sw1->FocusedBackgroundColor = 0x00FF00FF;
	sw1->Focusable = true;
	sw1->FocusIndex = 9;
	sw1->onCheckedChanged += [](bool value)
	{
		RenderController::getInstance().pushPopupMessage("Value is now: " + to_string(value), PopupMessageIcon::Info);
	};
	addChild(sw1);

	ProgressBar* pb = new ProgressBar();
	pb->setPosition(100, 600);
	pb->FocusedBackgroundColor = 0x00FF00FF;
	pb->Focusable = true;
	pb->FocusIndex = 10;
	pb->onValueChanged += [](int value)
	{
		RenderController::getInstance().pushPopupMessage("Value is now: " + to_string(value), PopupMessageIcon::Info);
	};
	addChild(pb);

	Button* btn = new Button("CLICCAMI!");
	btn->setPosition(100, 650);
	btn->FocusedBackgroundColor = 0x00FF00FF;
	btn->Focusable = true;
	btn->FocusIndex = 11;
	btn->onButtonPressed += [](int value)
	{
		RenderController::getInstance().pushPopupMessage("Button pressed", PopupMessageIcon::Info);
	};
	addChild(btn);

	InputController::getInstance().enableJoystickHandling();
}

bool TestWindow::input(Model::Input input)
{
	if(input.RawData.ValueID == SDLK_p && input.Value != SDL_RELEASED )
	{
		RenderController::getInstance().pushPopupMessage("Miao testo lungo lungo lungo lungo lungo lungo lungo lungo lungo!", PopupMessageIcon::Info);
		return true;
	}

	//override default method and send input only to focused child
	for(unsigned int i = getChildCount() - 1; getChildCount() > 0 && i > 0; i--)
	{
		BaseView* child = mChildren[i];
		
		if(mChildren[i]->Focused && mChildren[i]->input(input)) return true;
	}
	
	if(input.Semantic == InputSemantic::DOWN && input.Value != SDL_RELEASED )
	{
		focusNext();
	}

	if(input.Semantic == InputSemantic::UP && input.Value != SDL_RELEASED )
	{
		focusPrev();
	}
	return false;
}
