/* Text input dialog
*
* Create a Modal Dialog to let the user insert a text
* 
*/

#pragma once

#include "../../globals.h"

#include "../Model/InputConfig.h"

#include "BaseView.h"
#include "Label.h"

namespace RetroEnd
{
	namespace View
	{
		class TextInputDialog : public BaseView
		{
		
		public:
			TextInputDialog(string title, string value, function<void (string value)> callback, bool obfuscated = false);

			//OVERRIDES

			bool input(Model::Input input) override;

		private:
			function<void (string text)> mTextInputCallback;

			function<void (string text)> mCallback;

			BaseView* mBack;

			Label* mTitle;

			Label* mTextField;
		};
	}
}
