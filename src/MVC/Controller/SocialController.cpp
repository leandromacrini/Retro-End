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

	try
	{
		mConnection = mEndpoint.get_connection(mHandler->perform_handshake("ws://localhost"));
		mEndpoint.connect(mConnection);

		mThread = new boost::thread(boost::bind(&client::run, &mEndpoint, false));
		// Wait for a sec before sending stuff
		while (!mHandler->connected()) {
			Sleep(1);
		}

		//TODO Disconnect -> IsOnline = false
		//TODO add some LOG

		mHandler->bind_event("serverError", [this](socketio_events& evt, const Value& args)
		{
			string json = args[SizeType(0)].GetString();

			onSocialServerError(SocialBoolResponse(false, json));
		});

		mHandler->bind_event("loginResponse", [this](socketio_events& evt, const Value& args)
		{
			bool success = args[SizeType(0)]["success"].GetBool();
			string description = args[SizeType(0)]["description"].GetString();

			if(success)
			{
				//update Local User
				mLocalPlayer.ID = args[SizeType(0)]["result"]["id"].GetString();
				mLocalPlayer.Username = args[SizeType(0)]["result"]["username"].GetString();
				mLocalPlayer.EMail = args[SizeType(0)]["result"]["email"].GetString();
				mLocalPlayer.FirstName = args[SizeType(0)]["result"]["first_name"].GetString();
				mLocalPlayer.LastName = args[SizeType(0)]["result"]["last_name"].GetString();
				mLocalPlayer.IsOnline = true;
			}

			onDoneLogin(SocialBoolResponse(success, description));
			RenderController::getInstance().pushPopupMessage("Manta Social Login!", PopupMessageIcon::Info); //TODO move to mainwindow
		});

		mHandler->bind_event("logoutResponse", [this](socketio_events& evt, const Value& args)
		{
			bool success = args[SizeType(0)]["success"].GetBool();
			string description = args[SizeType(0)]["description"].GetString();

			if(success)
			{
				//update Local User
				mLocalPlayer.ID = args[SizeType(0)]["result"]["id"].GetString();
				mLocalPlayer.Username = args[SizeType(0)]["result"]["username"].GetString();
				mLocalPlayer.EMail = args[SizeType(0)]["result"]["email"].GetString();
				mLocalPlayer.FirstName = args[SizeType(0)]["result"]["first_name"].GetString();
				mLocalPlayer.LastName = args[SizeType(0)]["result"]["last_name"].GetString();
				mLocalPlayer.IsOnline = true;
			}

			onDoneLogout(SocialBoolResponse(success, description));
			RenderController::getInstance().pushPopupMessage("Manta Social Logout!", PopupMessageIcon::Info); //TODO move to mainwindow
		});

		mHandler->bind_event("getUserResponse", [this](socketio_events& evt, const Value& args)
		{
			bool success = args[SizeType(0)]["success"].GetBool();
			string description = args[SizeType(0)]["description"].GetString();

			User result;

			if(success)
			{
				result.ID = args[SizeType(0)]["result"]["id"].GetString();
				result.Username = args[SizeType(0)]["result"]["username"].GetString();
				result.FirstName = args[SizeType(0)]["result"]["first_name"].GetString();
				result.LastName = args[SizeType(0)]["result"]["last_name"].GetString();
				result.IsOnline = args[SizeType(0)]["result"]["online"].GetBool();
			}

			onDoneGetUser(SocialUserResponse(success, description, result));
		});

		mHandler->bind_event("addUserResponse", [this](socketio_events& evt, const Value& args)
		{
			bool success = args[SizeType(0)]["success"].GetBool();
			string description = args[SizeType(0)]["description"].GetString();

			User result;

			if(success)
			{
				result.ID = args[SizeType(0)]["result"]["id"].GetString();
				result.Username = args[SizeType(0)]["result"]["username"].GetString();
				result.FirstName = args[SizeType(0)]["result"]["first_name"].GetString();
				result.LastName = args[SizeType(0)]["result"]["last_name"].GetString();
			}

			onDoneAddUser(SocialUserResponse(success, description, result));
		});

		mHandler->bind_event("delUserResponse", [this](socketio_events& evt, const Value& args)
		{
			bool success = args[SizeType(0)]["success"].GetBool();
			string description = args[SizeType(0)]["description"].GetString();

			onDoneDelUser(SocialBoolResponse(success, description));
		});

		mHandler->bind_event("searchUsersResponse", [this](socketio_events& evt, const Value& args)
		{
			bool success = args[SizeType(0)]["success"].GetBool();
			string description = args[SizeType(0)]["description"].GetString();

			vector<User> users;

			if(success)
			{
				for(SizeType i = 0; i < args[SizeType(0)]["result"].Size(); i++)
				{
					User user;

					user.ID = args[SizeType(0)]["result"][i]["id"].GetString();
					user.Username = args[SizeType(0)]["result"][i]["username"].GetString();
					user.FirstName = args[SizeType(0)]["result"][i]["first_name"].GetString();
					user.LastName = args[SizeType(0)]["result"][i]["last_name"].GetString();

					users.push_back(user);
				}
			}

			onDoneSearchUsers(SocialUserListResponse(success, description, users));
		});

		mHandler->bind_event("addFriendResponse", [this](socketio_events& evt, const Value& args)
		{
			bool success = args[SizeType(0)]["success"].GetBool();
			string description = args[SizeType(0)]["description"].GetString();

			onDoneAddFriend(SocialBoolResponse(success, description));
		});

		mHandler->bind_event("delFriendResponse", [this](socketio_events& evt, const Value& args)
		{
			bool success = args[SizeType(0)]["success"].GetBool();
			string description = args[SizeType(0)]["description"].GetString();

			onDoneDelFriend(SocialBoolResponse(success, description));
		});

		mHandler->bind_event("approveFriendResponse", [this](socketio_events& evt, const Value& args)
		{
			bool success = args[SizeType(0)]["success"].GetBool();
			string description = args[SizeType(0)]["description"].GetString();

			onDoneApproveFriend(SocialBoolResponse(success, description));
		});

		mHandler->bind_event("updateFriends", [this](socketio_events& evt, const Value& args)
		{
			LOG(LogLevel::Info, "SocialController updateFriends event received");
			//when server send an updateFrieds event we ask for Friends
			doGetFriends();
		});

		mHandler->bind_event("getFriendsResponse", [this](socketio_events& evt, const Value& args)
		{
			bool success = args[SizeType(0)]["success"].GetBool();
			string description = args[SizeType(0)]["description"].GetString();

			mSocialFriends.clear();

			if(success)
			{
				for(SizeType i = 0; i < args[SizeType(0)]["result"]["Friends"].Size(); i++)
				{
					User user;

					user.ID = args[SizeType(0)]["result"]["Friends"][i]["id"].GetString();
					user.Username = args[SizeType(0)]["result"]["Friends"][i]["username"].GetString();
					user.IsOnline = args[SizeType(0)]["result"]["Friends"][i]["online"].GetBool();

					if( ! args[SizeType(0)]["result"]["Friends"][i]["first_name"].IsNull())
						user.FirstName = args[SizeType(0)]["result"]["Friends"][i]["first_name"].GetString();
					
					if( ! args[SizeType(0)]["result"]["Friends"][i]["last_name"].IsNull())
						user.LastName = args[SizeType(0)]["result"]["Friends"][i]["last_name"].GetString();
					

					mSocialFriends.push_back(user);
				}
			}

			mSocialRequests.clear();

			if(success)
			{
				for(SizeType i = 0; i < args[SizeType(0)]["result"]["Requests"].Size(); i++)
				{
					User user;

					user.ID = args[SizeType(0)]["result"]["Requests"][i]["user"]["id"].GetString();
					user.Username = args[SizeType(0)]["result"]["Requests"][i]["user"]["username"].GetString();
					
					if( ! args[SizeType(0)]["result"]["Friends"][i]["first_name"].IsNull())
						user.FirstName = args[SizeType(0)]["result"]["Requests"][i]["user"]["first_name"].GetString();
					
					if( ! args[SizeType(0)]["result"]["Friends"][i]["last_name"].IsNull())
						user.LastName = args[SizeType(0)]["result"]["Requests"][i]["user"]["last_name"].GetString();

					mSocialRequests.push_back(user);
				}
			}

			onDoneGetFriends(SocialBoolResponse(success, description));
			onFriendsUpdated(SocialBoolResponse(success, description));
		});

		mHandler->bind_event("sendPlayInvitationResponse", [this](socketio_events& evt, const Value& args)
		{
			bool success = args[SizeType(0)]["success"].GetBool();
			string description = args[SizeType(0)]["description"].GetString();

			onDoneSendPlayInvitation(SocialBoolResponse(success, description));
		});

		mHandler->bind_event("answerInvitationResponse", [this](socketio_events& evt, const Value& args)
		{
			bool success = args[SizeType(0)]["success"].GetBool();
			string description = args[SizeType(0)]["description"].GetString();

			onDoneAnswerPlayInvitation(SocialBoolResponse(success, description));
		});

		mHandler->bind_event("answerInvitation", [this](socketio_events& evt, const Value& args)
		{
			bool answer = args[SizeType(0)]["answer"].GetBool();
			string reason = args[SizeType(0)]["reason"].GetString();
			User user;
			user.Username = args[SizeType(0)]["username"].GetString();

			onAnswerInvitationReceived(SocialUserResponse(answer, reason, user));
		});

		mHandler->bind_event("playInvitation", [this](socketio_events& evt, const Value& args)
		{
			SocialInvitationRequest request;

			request.Username = args[SizeType(0)]["username"].GetString();
			request.IsLocal = args[SizeType(0)]["local"].GetBool();
			request.LocalAddress = args[SizeType(0)]["local_address"].GetString();
			request.WebAddress = args[SizeType(0)]["web_address"].GetString();
			request.Port = args[SizeType(0)]["port"].GetUint();

			onPlayInvitationReceived(request);
		});
	}
	catch(exception& ex)
	{
		LOG(LogLevel::Error,  "SocialController::activate error: " + string(ex.what()));
		RenderController::getInstance().pushPopupMessage("Unable to connect to Manta Social Server", PopupMessageIcon::Error); //TODO move to mainwindow
	}
}

void SocialController::deactivate()
{
	if(mActivated)
	{
		BaseController::deactivate();

		mEndpoint.stop(false); //TODO check if we can wait without exception
	}
}

const User SocialController::getLocalPlayer()
{
	return mLocalPlayer;
}

bool onlineSorter(User u1, User u2)
{
	if(u1.IsOnline && !u2.IsOnline) return true;
	
	return false;
}

bool nameSorter(User u1, User u2)
{
	return u1.Username < u2.Username;
}

const vector<User> SocialController::getSocialFriends()
{
	stable_sort(mSocialFriends.begin(), mSocialFriends.end(), nameSorter);
	stable_sort(mSocialFriends.begin(), mSocialFriends.end(), onlineSorter);

	return mSocialFriends;
}

const vector<User> SocialController::getSocialRequests()
{
	return mSocialRequests;
}

const bool SocialController::getIsLogged()
{
	return mLocalPlayer.IsOnline;
}

//async methods

void SocialController::doLogin(string username, string password)
{
	if( ! mHandler->connected() )
	{
		LOG(LogLevel::Error,  "SocialController::doLogin error: No connection available");
		RenderController::getInstance().pushPopupMessage("No connection available to Manta Social Server", PopupMessageIcon::Error);
	}
	else
	{
		//store data
		mLocalPlayer.Username = username;
		mLocalPlayer.Password = password;

		//emit event

		Document d;d.SetObject();

		Value v;v.SetObject();

		v.AddMember("username", username.c_str(), d.GetAllocator());
		v.AddMember("password", password.c_str(), d.GetAllocator());

		d.AddMember("args", v, d.GetAllocator());

		mHandler->emit("login", d);
	}
}

void SocialController::doLogout()
{
	if( ! mHandler->connected() || ! mLocalPlayer.IsOnline )
	{
		LOG(LogLevel::Error,  "SocialController::doLogout error: Not connected");
		RenderController::getInstance().pushPopupMessage("Not connected to Manta Social Server", PopupMessageIcon::Error);
	}
	else
	{
		Document d;d.SetObject();
		Value v;v.SetObject();

		v.AddMember("username", mLocalPlayer.Username.c_str(), d.GetAllocator());
		v.AddMember("password", mLocalPlayer.Password.c_str(), d.GetAllocator());
		d.AddMember("args", v, d.GetAllocator());

		mHandler->emit("logout", d);
	}
}

void SocialController::doGetUser(string username)
{
	if( ! mHandler->connected() )
	{
		LOG(LogLevel::Error,  "SocialController::doGetUser error: Not connected");
		RenderController::getInstance().pushPopupMessage("Not connected to Manta Social Server", PopupMessageIcon::Error);
	}
	else
	{
		Document d;d.SetObject();
		Value v;v.SetObject();

		v.AddMember("username", username.c_str(), d.GetAllocator());
		d.AddMember("args", v, d.GetAllocator());

		mHandler->emit("getUser", d);
	}
}

void SocialController::doAddUser(Model::User user)
{
	if( ! mHandler->connected() )
	{
		LOG(LogLevel::Error,  "SocialController::doDelUser error: Not connected");
		RenderController::getInstance().pushPopupMessage("Not connected to Manta Social Server", PopupMessageIcon::Error);
	}
	else
	{
		Document d;d.SetObject();
		Value v;v.SetObject();

		v.AddMember("username", user.Username.c_str(), d.GetAllocator());
		v.AddMember("password", user.Password.c_str(), d.GetAllocator());
		v.AddMember("first_name", user.FirstName.c_str(), d.GetAllocator());
		v.AddMember("last_name", user.LastName.c_str(), d.GetAllocator());
		v.AddMember("email", user.EMail.c_str(), d.GetAllocator());
		d.AddMember("args", v, d.GetAllocator());

		mHandler->emit("addUser", d);
	}
}

void SocialController::doDelUser(Model::User user)
{
	if( ! mHandler->connected() )
	{
		LOG(LogLevel::Error,  "SocialController::doDelUser error: Not connected");
		RenderController::getInstance().pushPopupMessage("Not connected to Manta Social Server", PopupMessageIcon::Error);
	}
	else
	{
		Document d;d.SetObject();
		Value v;v.SetObject();

		v.AddMember("username", user.Username.c_str(), d.GetAllocator());
		v.AddMember("password", user.Password.c_str(), d.GetAllocator());
		d.AddMember("args", v, d.GetAllocator());

		mHandler->emit("delUser", d);
	}
}

void SocialController::doSearchUsers(string search)
{
	if( ! mHandler->connected() )
	{
		LOG(LogLevel::Error,  "SocialController::doSearchUsers error: Not connected");
		RenderController::getInstance().pushPopupMessage("Not connected to Manta Social Server", PopupMessageIcon::Error);
	}
	else
	{
		Document d;d.SetObject();
		Value v;v.SetObject();

		v.AddMember("search", search.c_str(), d.GetAllocator());
		d.AddMember("args", v, d.GetAllocator());

		mHandler->emit("searchUsers", d);
	}
}

void SocialController::doAddFriend(Model::User user)
{
	if( ! mHandler->connected() || ! mLocalPlayer.IsOnline)
	{
		LOG(LogLevel::Error,  "SocialController::doAddFriend error: Not connected");
		RenderController::getInstance().pushPopupMessage("Not connected to Manta Social Server", PopupMessageIcon::Error);
	}
	else
	{
		Document d;d.SetObject();
		Value v;v.SetObject();

		v.AddMember("friend_id", user.ID.c_str(), d.GetAllocator());
		v.AddMember("friend_username", user.Username.c_str(), d.GetAllocator());
		d.AddMember("args", v, d.GetAllocator());

		mHandler->emit("addFriend", d);
	}
}

void SocialController::doDelFriend(Model::User user)
{
	if( ! mHandler->connected() || ! mLocalPlayer.IsOnline)
	{
		LOG(LogLevel::Error,  "SocialController::doDelFriend error: Not connected");
		RenderController::getInstance().pushPopupMessage("Not connected to Manta Social Server", PopupMessageIcon::Error);
	}
	else
	{
		Document d;d.SetObject();
		Value v;v.SetObject();

		v.AddMember("friend_id", user.ID.c_str(), d.GetAllocator());
		v.AddMember("friend_username", user.Username.c_str(), d.GetAllocator());
		d.AddMember("args", v, d.GetAllocator());

		mHandler->emit("delFriend", d);
	}
}

void SocialController::doApproveFriend(Model::User user)
{
	if( ! mHandler->connected() || ! mLocalPlayer.IsOnline)
	{
		LOG(LogLevel::Error,  "SocialController::doApproveFriend error: Not connected");
		RenderController::getInstance().pushPopupMessage("Not connected to Manta Social Server", PopupMessageIcon::Error);
	}
	else
	{
		Document d;d.SetObject();
		Value v;v.SetObject();

		v.AddMember("friend_id", user.ID.c_str(), d.GetAllocator());
		v.AddMember("friend_username", user.Username.c_str(), d.GetAllocator());
		d.AddMember("args", v, d.GetAllocator());

		mHandler->emit("approveFriend", d);
	}
}

void SocialController::doGetFriends()
{
	if( ! mHandler->connected() || ! mLocalPlayer.IsOnline )
	{
		LOG(LogLevel::Error,  "SocialController::doGetFriends error: Not connected");
		RenderController::getInstance().pushPopupMessage("Not connected to Manta Social Server", PopupMessageIcon::Error);
	}
	else
	{
		Document d;d.SetObject();
		Value v;v.SetObject();

		v.AddMember("user_id", mLocalPlayer.ID.c_str(), d.GetAllocator());
		d.AddMember("args", v, d.GetAllocator());

		mHandler->emit("getFriends", d);
	}
}

void SocialController::doSendPlayInvitation(Model::User user, bool local)
{
	if( ! mHandler->connected() || ! mLocalPlayer.IsOnline )
	{
		LOG(LogLevel::Error,  "SocialController::doSendPlayInvitation error: Not connected");
		RenderController::getInstance().pushPopupMessage("Not connected to Manta Social Server", PopupMessageIcon::Error);
	}
	else
	{
		Document d;d.SetObject();
		Value v;v.SetObject();

		string local_address = mConnection->get_origin();
		local_address = mConnection->get_host();
		local_address = mConnection->get_raw_socket().local_endpoint().address().to_string();

		v.AddMember("friend_username", user.Username.c_str(), d.GetAllocator());
		v.AddMember("game_port", mLocalPlayer.NetPort, d.GetAllocator());
		v.AddMember("local", local, d.GetAllocator());
		v.AddMember("local_address", local_address.c_str() , d.GetAllocator());
		d.AddMember("args", v, d.GetAllocator());

		mHandler->emit("sendPlayInvitation", d);
	}
}

void SocialController::doAnswerPlayInvitation(Model::User user, bool answer, string reason)
{
	if( ! mHandler->connected() || ! mLocalPlayer.IsOnline )
	{
		LOG(LogLevel::Error,  "SocialController::doAnswerPlayInvitation error: Not connected");
		RenderController::getInstance().pushPopupMessage("Not connected to Manta Social Server", PopupMessageIcon::Error);
	}
	else
	{
		Document d;d.SetObject();
		Value v;v.SetObject();

		v.AddMember("friend_username", user.Username.c_str(), d.GetAllocator());
		v.AddMember("answer", answer, d.GetAllocator());
		v.AddMember("reason", reason.c_str(), d.GetAllocator());
		d.AddMember("args", v, d.GetAllocator());

		mHandler->emit("answerPlayInvitation", d);
	}
}

