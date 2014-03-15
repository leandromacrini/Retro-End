#include "NetplayView.h"

#include "../Controller/RenderController.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

NetplayView::NetplayView(NetplayRequest request) : mRequest(request)
{
	float W = (float) RenderController::getInstance().getScreenWidth();
	float H = (float) RenderController::getInstance().getScreenHeight();

	//shadow
	mSize = Eigen::Vector2f(W,H);
	BackgroundColor = 0x1d1d1d77;

	//container
	mBack = new BaseView();
	mBack->BackgroundColor = 0xEDEDEDFF;
	mBack->setSize(W * 5/6, H * 5/6);
	mBack->setPosition(W/6 / 2, H);

	//title
	mTitle = new Label();
	mTitle->setSize( mBack->getSize().x(), 0);
	mTitle->BackgroundColor = 0x2B729533;
	mTitle->setFont(Font::get("data/fonts/eurof35.ttf", FONT_SIZE_LARGE));
	mTitle->HorizontalTextAlign = TextAlign::Center;
	mTitle->setText("NETPLAY");
	mBack->addChild(mTitle);

	//animation
	Animation* a = new Animation();
	a->moveOffset = Eigen::Vector3f(0, -(H * 5/6)-(H/6 / 2) - H/6 / 4, 0);
	a->millisDuration = 250;
	a->endCallback = [H, this] () 
	{
		Animation* a = new Animation();
		a->moveOffset = Eigen::Vector3f(0, H/6 / 4, 0);
		a->millisDuration = 100;
		mBack->animate(a);
	};
	mBack->animate(a);

	mRightFriends = new ListView();
	mRightFriends->setSize(mBack->getSize().x(), mBack->getSize().y() - mTitle->getSize().y());
	mRightFriends->setPosition(0, mTitle->getSize().y());
	mRightFriends->HorizontalTextAlign = TextAlign::Center;
	mRightFriends->TitleColor = 0x000000FF;
	mRightFriends->SelectedRowBackgroundColor = 0x1d1d1d99;
	mRightFriends->SelectedTitleColor = 0xFFFFFFFF;
	mBack->addChild(mRightFriends);

	SocialController::getInstance().onFriendsUpdated += [this](SocialBoolResponse)
	{
		setFriendsList(SocialController::getInstance().getSocialFriends());
	};

	this->addChild(mBack);

}

void NetplayView::setFriendsList(vector<Model::User> friends)
{
	mRightFriends->removeAllRows();
	for(Uint32 i = 0; i < friends.size(); i++)
	{
		mRightFriends->addRow(friends[i].Username + (friends[i].IsOnline? " (online)" : " (offline)"));
	}
}


NetplayView::~NetplayView()
{
	int a = 1;
}

void NetplayView::move(int direction)
{
	mRightFriends->setSelectedIndex( mRightFriends->getSelectedIndex() + direction );
}

bool NetplayView::input(Model::Input input)
{
	if(input.Semantic == InputSemantic::START && input.Value != SDL_RELEASED )
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
			};
			mBack->animate(a);
		};

		mBack->animate(a);

		return true;
	}

	if(input.Semantic == InputSemantic::UP && input.Value != SDL_RELEASED )
	{
		move(-1);
	}

	if(input.Semantic == InputSemantic::UP && input.Value != SDL_RELEASED )
	{
		move(1);
	}

	if(input.Semantic == InputSemantic::UP && input.Value != SDL_RELEASED )
	{
		User user;
		user.Username = mRightFriends->getSelectedTitle();
		SocialController::getInstance().doSendPlayInvitation(user, true);
		//SocialController::getInstance().
	}

	return false;
}
