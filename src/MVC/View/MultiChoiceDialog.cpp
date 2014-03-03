#include "MultiChoiceDialog.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

MultiChoiceDialog::MultiChoiceDialog() : BaseView(),  mValues(NULL)
{
	float W = (float) RenderController::getInstance().getScreenWidth();
	float H = (float) RenderController::getInstance().getScreenHeight();

	mSize = Eigen::Vector2f(W,H);
	mBackgroundColor = 0x1d1d1d99;

	mBack = new BaseView();
	mBack->setBackgroundColor(0xEDEDEDFF);
	mBack->setSize(W * 5/10, H * 8/10);
	mBack->setPosition(W* 2.5f/10, H);
	addChild(mBack);
	
	mTitle = new Label();
	mTitle->setSize(mBack->getSize().x(), 0);
	mTitle->HorizontalTextAlign = TextAlign::Center;
	mBack->addChild(mTitle);
	
	mMessage = new Label();
	mMessage->setSize(mBack->getSize().x(), 0);
	mMessage->setPosition(0, mTitle->getFont()->getSize() * 2.0f);
	mMessage->HorizontalTextAlign = TextAlign::Center;
	mMessage->WrapText = true;
	mBack->addChild(mMessage);

	mValuesList = new ListView();
	mValuesList->setSize(mBack->getSize().x(), mBack->getSize().y()/2);
	mValuesList->setPosition(0, mBack->getSize().y()/2);
	mValuesList->HorizontalTextAlign = TextAlign::Center;
	mValuesList->TitleColor = 0x000000FF;
	mValuesList->SelectedRowBackgroundColor = 0x1d1d1d99;
	mValuesList->SelectedTitleColor = 0xFFFFFFFF;
	mBack->addChild(mValuesList);
	
	Animation* a = new Animation();
	a->moveOffset = new Eigen::Vector3f(0, -(H *9/10) -50, 0);
	a->millisDuration = 250;
	a->endCallback = [H, this] () 
	{
		Animation* a = new Animation();
		a->moveOffset = new Eigen::Vector3f(0, 50, 0);
		a->millisDuration = 100;
		mBack->animate(a);
	};
	mBack->animate(a);
}

void MultiChoiceDialog::showDialog(string message, string title, vector<string>* values, function<void (unsigned int selectedIndex)> callback)
{
	mTitle->setText(title);
	mMessage->setText(message);
	mCallback = callback;
	mValues = values;
	
	for(unsigned int i = 0; i < mValues->size(); i++)
	{
		mValuesList->addRow(mValues->at(i));
	}
}

void MultiChoiceDialog::move(int direction)
{
	mValuesList->setSelectedIndex( mValuesList->getSelectedIndex() + direction );
}

bool MultiChoiceDialog::input(Input input)
{
	//TODO input with settings
	if(input.Semantic == InputSemantic::UP && input.Value != SDL_RELEASED )
	{
		move(-1);
		return true;
	}

	if(input.Semantic == InputSemantic::DOWN && input.Value != SDL_RELEASED )
	{
		move(1);
		return true;
	}

	if(input.Semantic == InputSemantic::BUTTON_A && input.Value != SDL_RELEASED )
	{
		if(mCallback) mCallback(mValuesList->getSelectedIndex());
		return true;
	}

	return false;
}