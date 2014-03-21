#include "NetplayView.h"

#include <boost/function.hpp>

#include "Switch.h"
#include "Button.h"
#include "TextInputDialog.h"
#include "MultiChoiceDialog.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

NetplayView::NetplayView(NetplayRequest request) : mRequest(request)
{
	float W = (float) RenderController::getInstance().getScreenWidth();
	float H = (float) RenderController::getInstance().getScreenHeight();

	//attach handlers
	SocialController::getInstance().onDoneLogin += boost::bind(&NetplayView::loginHandler, this, _1);
	SocialController::getInstance().onDoneAddUser += boost::bind(&NetplayView::registrationHandler, this, _1);
	SocialController::getInstance().onDoneSearchUsers += boost::bind(&NetplayView::searchFriendHandler, this, _1);
	SocialController::getInstance().onFriendsUpdated += boost::bind(&NetplayView::updateFriendsHandler, this, _1);

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

	//create options list
	mLeftOptions = new OptionsListView();
	mLeftOptions->setSize(mBack->getSize().x()/2, mBack->getSize().y() - mTitle->getSize().y());
	mLeftOptions->setPosition(0, mTitle->getSize().y());
	mLeftOptions->Focused = true;
	mBack->addChild(mLeftOptions);

	//friends list
	mRightFriends = new ListView();
	mRightFriends->setSize(mBack->getSize().x()/2, mBack->getSize().y() - mTitle->getSize().y());
	mRightFriends->setPosition(mBack->getSize().x()/2, mTitle->getSize().y());
	mRightFriends->HorizontalTextAlign = TextAlign::Center;
	mRightFriends->TitleColor = 0x000000FF;
	mRightFriends->SelectedRowBackgroundColor = 0x1d1d1d99;
	mRightFriends->SelectedTitleColor = 0xFFFFFFFF;
	mBack->addChild(mRightFriends);

	//if logged in update friends list
	setFriendsList(SocialController::getInstance().getSocialFriends());
	this->addChild(mBack);

	//MANTA SOCIAL SWITCH
	Switch* manta = new Switch(SocialController::getInstance().getIsConnected());
	manta->onCheckedChanged += [this](bool value)
	{
		if(value)
		{
			RenderController::getInstance().pushPopupMessage("Connecting to MANTA Social Server ...", PopupMessageIcon::Info);
			SocialController::getInstance().activate();
		}
		else
		{
			RenderController::getInstance().pushPopupMessage("MANTA Social Server connection turned off", PopupMessageIcon::Info);
			SocialController::getInstance().deactivate();
		}
	};
	mLeftOptions->addRow("MANTA Social Server activation", manta);

	Label* login = new Label();
	login->Focusable = true;
	login->onLabelPressed += [this, login] (int)
	{
		//ask username
		TextInputDialog* dialog = new TextInputDialog("Enter Username", login->getText(), [this] (string username)
		{	
			//ask password
			TextInputDialog* dialog = new TextInputDialog("Enter Password", "", [this, username] (string password)
			{
				SocialController::getInstance().doLogin(username, password);
			}, true);
		});
	};
	mLeftOptions->addRow("Login", login);

	Label* logout = new Label();
	logout->Focusable = true;
	logout->onLabelPressed += [this, login] (int)
	{
		//logout
		if(SocialController::getInstance().getLocalPlayer().IsOnline)
		{
			SocialController::getInstance().doLogout();
			RenderController::getInstance().pushPopupMessage("User logged out", PopupMessageIcon::Info);
		}
		else
		{
			RenderController::getInstance().pushPopupMessage("You are not logged in", PopupMessageIcon::Error);
		}
		mRightFriends->removeAllRows();
	};
	mLeftOptions->addRow("Logout", logout);

	Label* reg = new Label();
	reg->Focusable = true;
	reg->onLabelPressed += [this, login] (int)
	{
		//ask email
		TextInputDialog* dialog = new TextInputDialog("Enter registration eMail", login->getText(), [this, login] (string email)
		{	
			//ask username
			TextInputDialog* dialog = new TextInputDialog("Enter registration Username", login->getText(), [this, login, email] (string username)
			{	
				//ask password
				TextInputDialog* dialog = new TextInputDialog("Enter registration Password", "", [this, email, username] (string password)
				{
					User user;
					user.EMail = email;
					user.Username = username;
					user.Password = password;

					SocialController::getInstance().doAddUser(user);
				}, true);
			});
		});
	};
	mLeftOptions->addRow("Register", reg);

	Label* addFriend = new Label();
	addFriend->Focusable = true;
	addFriend->onLabelPressed += [this, login] (int)
	{
		//ask email
		TextInputDialog* dialog = new TextInputDialog("Enter the user to search for", login->getText(), [this, login] (string username)
		{	
			SocialController::getInstance().doSearchUsers(username);
		});
	};
	mLeftOptions->addRow("Search friends", addFriend);

	Label* delFriend = new Label();
	delFriend->Focusable = true;
	delFriend->onLabelPressed += [this, login] (int)
	{
		RenderController::getInstance().pushPopupMessage("Not implementes yet!", PopupMessageIcon::Warning);
	};
	mLeftOptions->addRow("Delete a friend (TODO)", delFriend);

	Label* lanPlay = new Label();
	lanPlay->Focusable = true;
	lanPlay->onLabelPressed += [this, login] (int)
	{
		vector<User> users = SocialController::getInstance().getSocialFriendsOnline();
		//if none is online abort
		if(users.size() ==0)
		{
			RenderController::getInstance().pushPopupMessage("There are no friends online", PopupMessageIcon::Warning);
			return;
		}

		MultiChoiceDialog* mDialog = new MultiChoiceDialog();
		vector<string>* values = new vector<string>();
		for(auto it = users.begin(); it != users.end(); it++)
		{
			values->push_back(it->Username);
		}

		//add the "cancel" row
		values->push_back("Abort operation and return");

		RenderController::getInstance().getCurrentWindow()->addChild(mDialog);
		mDialog->showDialog("Select the friend to invitate to play", "Play invitation (LAN)", values, [this, users, values, mDialog] (unsigned int index)
		{
			//if not aborted
			if(index < users.size())
			{
				SocialController::getInstance().doSendPlayInvitation(users.at(index), mRequest.NetDevice, mRequest.NetGame, true);
			}
			RenderController::getInstance().getCurrentWindow()->removeChild(mDialog);
			delete values;
		});
	};
	mLeftOptions->addRow("Send play invitation (LAN)", lanPlay);

	Label* webPlay = new Label();
	webPlay->Focusable = true;
	webPlay->onLabelPressed += [this, login] (int)
	{
		vector<User> users = SocialController::getInstance().getSocialFriendsOnline();
		//if none is online abort
		if(users.size() ==0)
		{
			RenderController::getInstance().pushPopupMessage("There are no friends online", PopupMessageIcon::Warning);
			return;
		}

		MultiChoiceDialog* mDialog = new MultiChoiceDialog();
		vector<string>* values = new vector<string>();
		for(auto it = users.begin(); it != users.end(); it++)
		{
			values->push_back(it->Username);
		}

		//add the "cancel" row
		values->push_back("Abort operation and return");

		RenderController::getInstance().getCurrentWindow()->addChild(mDialog);
		mDialog->showDialog("Select the friend to invitate to play", "Play invitation (LAN)", values, [this, users, values, mDialog] (unsigned int index)
		{
			//if not aborted
			if(index < users.size())
			{
				SocialController::getInstance().doSendPlayInvitation(users.at(index), mRequest.NetDevice, mRequest.NetGame, false);
			}
			RenderController::getInstance().getCurrentWindow()->removeChild(mDialog);
			delete values;
		});
	};
	mLeftOptions->addRow("Send play invitation (WEB)", webPlay);

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

}

NetplayView::~NetplayView()
{
	SocialController::getInstance().onDoneLogin -= boost::bind(&NetplayView::loginHandler, this, _1);
	SocialController::getInstance().onDoneAddUser -= boost::bind(&NetplayView::registrationHandler, this, _1);
	SocialController::getInstance().onDoneSearchUsers -= boost::bind(&NetplayView::searchFriendHandler, this, _1);
	SocialController::getInstance().onFriendsUpdated -= boost::bind(&NetplayView::updateFriendsHandler, this, _1);
}

void NetplayView::updateFriendsHandler(Controller::SocialBoolResponse sbr)
{
	setFriendsList(SocialController::getInstance().getSocialFriends());

	vector<User> requests = SocialController::getInstance().getSocialRequests();

	for(auto it = requests.begin(); it != requests.end(); it++)
	{
		//TODO make this "async" for each request

		User user = *it;
		MultiChoiceDialog* mDialog = new MultiChoiceDialog();
		vector<string>* values = new vector<string>();

		//add the "cancel" row
		values->push_back("Yes, add as friend");
		values->push_back("No, thanks");

		RenderController::getInstance().getCurrentWindow()->addChild(mDialog);
		mDialog->showDialog("The user: " + it->Username + " want to add you as friend", "Friendship request", values, [this, values, mDialog, user] (unsigned int index)
		{
			//yes
			if(index == 0)
			{
				SocialController::getInstance().doApproveFriend(user);
			}
			//no
			else
			{
				//TODO send RefuseFriend
			}
			RenderController::getInstance().getCurrentWindow()->removeChild(mDialog);
			delete values;
		});
	}
}

void NetplayView::loginHandler(SocialBoolResponse sbr)
{
	if(sbr.Success)
	{
		RenderController::getInstance().pushPopupMessage("Login complete", PopupMessageIcon::Info);
	}
	else
	{
		RenderController::getInstance().pushPopupMessage(sbr.Description, PopupMessageIcon::Error);
	}
}

void NetplayView::registrationHandler(SocialUserResponse sur)
{
	if(sur.Success)
	{
		RenderController::getInstance().pushPopupMessage("New user correctly created", PopupMessageIcon::Info);
	}
	else
	{
		RenderController::getInstance().pushPopupMessage(sur.Description, PopupMessageIcon::Error);
	}
}

void NetplayView::searchFriendHandler(Controller::SocialUserListResponse sulr)
{

	MultiChoiceDialog* mDialog = new MultiChoiceDialog();

	vector<string>* values = new vector<string>();

	for(auto it = sulr.Users.begin(); it != sulr.Users.end(); it++)
	{
		values->push_back(it->Username);
	}

	//add the "cancel" row
	values->push_back("Abort operation and return");

	RenderController::getInstance().getCurrentWindow()->addChild(mDialog);
	mDialog->showDialog("Select the user to add as friend", "Friends found", values, [this, sulr,values, mDialog] (unsigned int index)
	{
		//if not aborted
		if(index < sulr.Users.size())
		{
			SocialController::getInstance().doAddFriend(sulr.Users.at(index));
		}
		RenderController::getInstance().getCurrentWindow()->removeChild(mDialog);
		delete values;
	});
}

void NetplayView::setFriendsList(vector<Model::User> friends)
{
	mRightFriends->removeAllRows();
	for(Uint32 i = 0; i < friends.size(); i++)
	{
		mRightFriends->addRow(friends[i].Username + (friends[i].IsOnline? " (online)" : " (offline)"));
	}
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

	BaseView::input(input);

	return true; //modal dialog absorb any input
}
