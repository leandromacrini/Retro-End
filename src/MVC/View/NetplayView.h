/* Netplay View
*
* Shows the Netplay options
*/


#pragma once

#include "../../globals.h"

#include "../Controller/SocialController.h"

#include "../Model/Game.h"
#include "../Model/Device.h"

#include "BaseView.h"
#include "Image.h"
#include "Label.h"
#include "ListView.h"

namespace RetroEnd
{
	namespace View
	{
		struct NetplayRequest
		{
			Model::Device NetDevice;
			Model::Game NetGame;

			NetplayRequest(Model::Device device, Model::Game game) : NetDevice(device), NetGame(game) {}
		};

		class NetplayView : public BaseView
		{

		public:
			NetplayView(NetplayRequest request);
			~NetplayView();

			bool input(Model::Input input) override;
			void setFriendsList(vector<Model::User> friends);

		private:
			NetplayRequest mRequest;
			View::BaseView* mBack;
			View::Label* mTitle;

			View::ListView* mLeftButtons;
			View::ListView* mRightFriends;

			void move(int direction);
		};
	}
}
