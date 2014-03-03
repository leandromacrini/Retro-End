#include "SocialController.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::Controller;

void SocialController::start()
{
	mEndpoint.elog().set_level(websocketpp::log::elevel::RERROR);
	mEndpoint.elog().set_level(websocketpp::log::elevel::FATAL);
	mEndpoint.elog().set_level(websocketpp::log::elevel::WARN);
	mEndpoint.alog().set_level(websocketpp::log::alevel::DEVEL);
}

void SocialController::stop()
{
	deactivate();
}

void SocialController::activate()
{
	BaseController::activate();

	mConnection = mEndpoint.get_connection(mHandler->perform_handshake("ws://localhost"));
	mEndpoint.connect(mConnection);

	mThread = new boost::thread(boost::bind(&client::run, &mEndpoint, false));
	// Wait for a sec before sending stuff
	while (!mHandler->connected()) {
		Sleep(1);
	}

	mHandler->bind_event("serverError", [this](socketio_events& evt, const Value& args)
	{
		string json = args[SizeType(0)].GetString();

		onSocialServerError(json);
	});

	mHandler->bind_event("loginResponse", [this](socketio_events& evt, const Value& args)
	{
		RenderController::getInstance().pushPopupMessage("Manta Social Login!", PopupMessageIcon::Info);

		onDoneLogin(1);
	});
}

void SocialController::deactivate()
{
	if(mActivated)
	{
		BaseController::deactivate();

		mEndpoint.stop(false); //TODO check if we can wait without exception
	}
}

const Model::User SocialController::getLocalPlayer()
{
	return mLocalPlayer;
}

const vector<Model::User> SocialController::getFriends()
{
	return mFriends;
}

//async methods
void SocialController::doLogin(string username, string password)
{
	Document d;d.SetObject();

	Value v;v.SetObject();

	v.AddMember("username", username.c_str(), d.GetAllocator());
	v.AddMember("password", password.c_str(), d.GetAllocator());

	d.AddMember("args", v, d.GetAllocator());

	mHandler->emit("login", d);
}

void SocialController::doLogout()
{

}

void SocialController::doGetUser(string username)
{

}

void SocialController::doAddUser(Model::User)
{

}

void SocialController::doDelUser(Model::User)
{

}

void SocialController::doSearchUsers(string search)
{

}

void SocialController::doAddFriend(Model::User user)
{

}

void SocialController::doDelFriend(Model::User user)
{

}

void SocialController::doApproveFriend(Model::User user)
{

}

void SocialController::doUpdateFriends()
{

}

void SocialController::doSendPlayInvitation(Model::User user)
{

}

void SocialController::doAnswerPlayInvitation(Model::User user)
{

}

