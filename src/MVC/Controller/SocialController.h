/* Social Controller
 *
 */

#pragma once

#include <socket.io-clientpp/socket_io_client.hpp>

#include "../../globals.h"

#include "BaseController.h"
#include "LogController.h"
#include "RenderController.h"

#include "../Model/User.h"
#include "../Model/Observer.h"

using socketio::socketio_client_handler;
using socketio::socketio_events;
using websocketpp::client;

using namespace RetroEnd;
using namespace socketio;

namespace RetroEnd
{
	namespace Controller
	{
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
			const vector<Model::User> getFriends();

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
			void doUpdateFriends();

			void doSendPlayInvitation(Model::User user);
			void doAnswerPlayInvitation(Model::User user);

			//EVENTS

			Model::Observer<int> onDoneLogin;
			Model::Observer<int> onDoneLogout;

			Model::Observer<Model::User> onDoneGetUser;
			Model::Observer<Model::User> onDoneAddUser;
			Model::Observer<Model::User> onDoneDelUser;
			Model::Observer<vector<Model::User>> onDoneSearchUsers;

			Model::Observer<int> onDoneAddFriend;
			Model::Observer<int> onDoneDelFriend;
			Model::Observer<int> onDoneApproveFriend;
			Model::Observer<vector<Model::User>> onDoneFriendsUpdate;

			Model::Observer<bool> onAnswerPlayInvitationReceived;

			Model::Observer<string> onSocialServerError;

		protected:

		private:
			Model::User mLocalPlayer;
			vector<Model::User> mFriends;

			//connection objects
			socketio_client_handler_ptr mHandler;
			client::connection_ptr mConnection;
			client mEndpoint;

			boost::thread* mThread;

			SocialController mInstance();
			SocialController() : mHandler(new socketio_client_handler()), mEndpoint(mHandler) {}; //private instance costructor for Singleton Controller
			SocialController(SocialController const&);// Don't Implement
			void operator=(SocialController const&); // Don't implement
		};
	}
}
