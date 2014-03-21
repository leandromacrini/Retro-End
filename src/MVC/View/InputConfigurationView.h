/* Input Configuration View
*
*  Map the conotroller buttons to the input internal semantic
*/


#pragma once

#include "../../globals.h"

#include "../Model/InputConfig.h"

#include "BaseView.h"
#include "Image.h"
#include "Label.h"

#define BUTTON_TIMEOUT 5000

namespace RetroEnd
{
	namespace View
	{
		class InputConfigurationView : public BaseView
		{

		public:
			InputConfigurationView(Model::InputConfig* config);

			bool input(Model::Input input) override;

			void update(unsigned int deltaTime) override;

		private:
			unsigned int mElapsedTime;

			//used for avoid multi axis bug on some controller and multi pressure of buttons
			unsigned int mLastTimeStamp;

			Model::InputConfig* mConfig;

			Model::InputSemantic mCurrentButton;

			BaseView* mBack;
			Label* mTitle;
			Label* mDescription;
			Label* mButton;

			void configureNextButton();
		};
	}
}
