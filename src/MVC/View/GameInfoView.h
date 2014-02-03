/* Base View class
*
* Create the interface for access to any GUI component
* Implements the animation system
* Implements the rendering and update systems
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
			~GameInfoView();

			bool input(Model::InputConfig* config, Model::Input input) override;

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
