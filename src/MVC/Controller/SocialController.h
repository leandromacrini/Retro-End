/* Social Controller
 *
 */

#pragma once

#include <socket.io-clientpp/socket_io_client.hpp>

#include "../../globals.h"

#include "BaseController.h"
#include "LogController.h"
#include "RenderController.h"

#include "../Model/Device.h"
#include "../Model/Game.h"
#include "../Model/Observer.h"

using socketio::socketio_client_handler;
using socketio::socketio_events;
using websocketpp::client;

using namespace RetroEnd;
using namespace socketio;

namespace RetroEnd
{
	namespace Model
	{
		struct User
		{
			string ID;
			string Username;
			string Password;
			string FirstName;
			string LastName;
			string EMail;

			string NetAddress;
			Uint32 NetPort;

			bool IsOnline;

			User() : NetPort(55435), IsOnline(false)
			{

			}
		};
	}

	namespace Controller
	{
		struct SocialBoolResponse
		{
			bool Success;
			string Description;

			SocialBoolResponse(bool success, string description) : Success(success), Description(description) { }
		};

		struct SocialUserResponse
		{
			bool Success;
			string Description;
			Model::User UserData;

			SocialUserResponse(bool success, string description, Model::User user) : Success(success), Description(description), UserData(user) { }
		};

		struct SocialUserListResponse
		{
			bool Success;
			string Description;
			vector<Model::User> Users;

			SocialUserListResponse(bool success, string description, vector<Model::User> users) : Success(success), Description(description), Users(users) { }
		};

		struct SocialInvitationRequest
		{
			string Username;
			bool IsLocal;
			string LocalAddress;
			string WebAddress;
			Uint32 Port;
			Model::Game* Game;
			Model::Device* Device;
		};

		struct SocialInvitationResponse
		{
			bool Accepted;
			string Description;
			string Username;
			Uint32 Port;
			Model::Game* Game;
			Model::Device* Device;
		};

		class SocialController : public BaseController
		{

		public:
			static SocialController& getInstance()
			{
				static SocialController instance;
				return instance;
			}

			void start();
			void stop();

			void activate();
			void deactivate();

			const Model::User getLocalPlayer();
			const vector<Model::User> getSocialFriends();
			const vector<Model::User> getSocialFriendsOnline();
			const vector<Model::User> getSocialRequests();
			const bool getIsLogged();
			const bool getIsConnected();

			//async methods
			void doLogin(string username, string password);
			void doLogout();

			void doGetUser(string username);
			void doAddUser(Model::User);
			void doDelUser(Model::User);
			void doSearchUsers(string search);

			void doAddFriend(Model::User user);
			void doDelFriend(Model::User user);
			void doApproveFriend(Model::User user);
			void doGetFriends();

			void doSendPlayInvitation(Model::User user, Model::Device device, Model::Game game, bool local);
			void doAnswerPlayInvitation(string username, bool answer, string reason, Model::Device* device, Model::Game* game);

			//EVENTS

			Model::Observer<SocialBoolResponse> onDoneLogin;
			Model::Observer<SocialBoolResponse> onDoneLogout;

			Model::Observer<SocialUserResponse> onDoneGetUser;
			Model::Observer<SocialUserResponse> onDoneAddUser;
			Model::Observer<SocialBoolResponse> onDoneDelUser;
			Model::Observer<SocialUserListResponse> onDoneSearchUsers;

			Model::Observer<SocialBoolResponse> onDoneAddFriend;
			Model::Observer<SocialBoolResponse> onDoneDelFriend;
			Model::Observer<SocialBoolResponse> onDoneApproveFriend;
			Model::Observer<SocialBoolResponse> onDoneGetFriends;

			Model::Observer<SocialBoolResponse> onFriendsUpdated;

			Model::Observer<SocialBoolResponse> onDoneSendPlayInvitation;
			Model::Observer<SocialBoolResponse> onDoneAnswerPlayInvitation;
			
			Model::Observer<SocialInvitationResponse> onAnswerInvitationReceived;
			Model::Observer<SocialInvitationRequest> onPlayInvitationReceived;

			Model::Observer<SocialBoolResponse> onSocialServerError;

		protected:

		private:
			bool mIsLogged;
			Model::User mLocalPlayer;
			vector<Model::User> mSocialFriends;
			vector<Model::User> mSocialRequests;

			//connection objects
			socketio_client_handler_ptr mHandler;
			client::connection_ptr mConnection;
			client mEndpoint;

			boost::thread* mThread;

			SocialController mInstance();
			SocialController() : mHandler(new socketio_client_handler()), mEndpoint(mHandler) {
				mLocalPlayer.IsOnline = false;
			}; //private instance costructor for Singleton Controller
			SocialController(SocialController const&);// Don't Implement
			void operator=(SocialController const&); // Don't implement
		};
	}
}
