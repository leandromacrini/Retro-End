/* Console Info View
*
* Shows the information of the selected game
*/


#pragma once

#include "../../globals.h"

#include "../Model/Device.h"

#include "BaseView.h"
#include "Image.h"
#include "Label.h"

namespace RetroEnd
{
	namespace View
	{
		class ConsoleInfoView : public BaseView
		{

		public:
			ConsoleInfoView(Model::Device& device);
			~ConsoleInfoView();

			bool input(Model::Input input) override;

		private:
			Model::Device& mDevice;
			BaseView* mBack;
			Image* mGameBack;
			Label* mTitle;
			Label* mDescription;
		};
	}
}
