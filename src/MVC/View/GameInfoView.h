/* Game Info View
*
* Shows the information of the selected game
*/


#pragma once

#include "../../globals.h"

#include "../Model/Game.h"

#include "BaseView.h"
#include "Image.h"
#include "Label.h"

namespace RetroEnd
{
	namespace View
	{
		class GameInfoView : public BaseView
		{

		public:
			GameInfoView(Model::Game& game);

			bool input(Model::Input input) override;

		private:
			Model::Game& mGame;
			BaseView* mBack;
			Image* mGameBack;
			Image* mESRB;
			Label* mTitle;
			Label* mDescription;
		};
	}
}
