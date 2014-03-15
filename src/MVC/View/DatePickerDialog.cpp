#include "DatePickerDialog.h"

#include <boost/algorithm/string.hpp>

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

vector<string>* DatePickerDialog::YEARS = NULL;
vector<string>* DatePickerDialog::MONTHS = NULL;
vector<string>* DatePickerDialog::DAYS = NULL;

DatePickerDialog::DatePickerDialog(string title, string date, function<void (string date)> callback) : BaseView(),
	mDate(date),
	mCallback(callback)
{
	float W = (float) RenderController::getInstance().getScreenWidth();
	float H = (float) RenderController::getInstance().getScreenHeight();

	RenderController::getInstance().getCurrentWindow()->addChild(this);
	Focused = true;

	//first instance set the static elements
	if( ! YEARS )
	{
		YEARS = new vector<string>();
		for(int i = 1950; i < 2050; i++)
		{
			YEARS->push_back(to_string(i));
		}
		
		MONTHS = new vector<string>();
		for(int i = 1; i < 13; i++)
		{
			MONTHS->push_back(to_string(i));
		}
		
		DAYS = new vector<string>();
		for(int i = 1; i < 32; i++)
		{
			DAYS->push_back(to_string(i));
		}
	}

	mSize = Eigen::Vector2f(W,H);
	BackgroundColor = 0x1d1d1d99;

	mBack = new BaseView();
	mBack->BackgroundColor = 0xEDEDEDFF;
	mBack->setSize(W * 5/10, H * 6/10);
	mBack->setPosition(W* 2.5f/10, H);
	addChild(mBack);
	
	mTitle = new Label();
	mTitle->setSize(mBack->getSize().x(), 0);
	mTitle->HorizontalTextAlign = TextAlign::Center;
	mTitle->setText(title);
	mBack->addChild(mTitle);

	mMonth = new ListView();
	mMonth->setSize(mBack->getSize().x()/3, mBack->getSize().y()/2);
	mMonth->setPosition(0, mBack->getSize().y()/4);
	mMonth->HorizontalTextAlign = TextAlign::Center;
	mMonth->TitleColor = 0x000000FF;
	mMonth->SelectedRowBackgroundColor = 0x1d1d1d99;
	mMonth->SelectedTitleColor = 0xFFFFFFFF;
	mMonth->setAllRows(MONTHS);
	mMonth->FocusIndex = 0;
	mMonth->Focused = true;
	mBack->addChild(mMonth);

	mDay = new ListView();
	mDay->setSize(mBack->getSize().x()/3, mBack->getSize().y()/2);
	mDay->setPosition(mBack->getSize().x()/3, mBack->getSize().y()/4);
	mDay->HorizontalTextAlign = TextAlign::Center;
	mDay->TitleColor = 0x000000FF;
	mDay->SelectedRowBackgroundColor = 0x1d1d1d99;
	mDay->SelectedTitleColor = 0xFFFFFFFF;
	mDay->setAllRows(DAYS);
	mDay->FocusIndex = 1;
	mBack->addChild(mDay);

	mYear = new ListView();
	mYear->setSize(mBack->getSize().x()/3, mBack->getSize().y()/2);
	mYear->setPosition(mBack->getSize().x()*2/3, mBack->getSize().y()/4);
	mYear->HorizontalTextAlign = TextAlign::Center;
	mYear->TitleColor = 0x000000FF;
	mYear->SelectedRowBackgroundColor = 0x1d1d1d99;
	mYear->SelectedTitleColor = 0xFFFFFFFF;
	mYear->setAllRows(YEARS);
	mYear->FocusIndex = 2;
	mBack->addChild(mYear);

	//parse date
	vector<std::string> splitted;
	boost::split(splitted, date, boost::is_any_of("/"));

	if(splitted.size() == 1) //only year
	{
		mYear->setSelectedIndex( 1950 + atoi(splitted[0].c_str()));
	}
	else if(splitted.size() == 3) //complete date
	{
		mYear->setSelectedIndex	( atoi(splitted[2].c_str()) - 1950);
		mMonth->setSelectedIndex( atoi(splitted[0].c_str()) - 1);
		mDay->setSelectedIndex	( atoi(splitted[1].c_str()) - 1);
	}
	
	Animation* a = new Animation();
	a->moveOffset = Eigen::Vector3f(0, -(H *8/10) -50, 0);
	a->millisDuration = 250;
	a->endCallback = [H, this] () 
	{
		Animation* a = new Animation();
		a->moveOffset = Eigen::Vector3f(0, 50, 0);
		a->millisDuration = 100;
		mBack->animate(a);
	};
	mBack->animate(a);
}

bool DatePickerDialog::input(Input input)
{
	if(input.Semantic == InputSemantic::RIGHT && input.Value != SDL_RELEASED )
	{
		mBack->focusNext();
		return true;
	}

	if(input.Semantic == InputSemantic::LEFT && input.Value != SDL_RELEASED )
	{
		mBack->focusPrev();
		return true;
	}

	if(input.Semantic == InputSemantic::BUTTON_A && input.Value != SDL_RELEASED )
	{
		if(mBack->isAnimating()) return false;

		float H = (float) RenderController::getInstance().getScreenHeight();

		//EXIT ANIMATION
		Animation* a = new Animation();
		a->moveOffset = Eigen::Vector3f(0, -H/6 / 4, 0);
		a->millisDuration = 100;
		a->endCallback = [this] () 
		{
			Animation* a = new Animation();
			a->moveOffset = Eigen::Vector3f(0, mBack->getSize().y(), 0);
			a->millisDuration = 250;
			a->endCallback = [this] () 
			{
				this->mParent->removeChild(this);
				if(mCallback) mCallback(mMonth->getSelectedTitle() + "/" + mDay->getSelectedTitle() + "/" + mYear->getSelectedTitle());
			};
			mBack->animate(a);
		};

		mBack->animate(a);

		return true;
	}

	return BaseView::input(input);
}