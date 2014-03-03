/* Help View
*
* Shows the help screen
*/


#pragma once

#include "../../globals.h"

#include "BaseView.h"
#include "Image.h"
#include "Label.h"

namespace RetroEnd
{
	namespace View
	{
		enum HelpScreen {
			CONSOLE_HELP,
			GAME_HELP
		};

		class HelpView : public BaseView
		{

		public:
			HelpView(HelpScreen screen);
			~HelpView();

			bool input(Model::Input input) override;

		private:
			Image* mBack;
			Label* mTitle;
		};
	}
}
