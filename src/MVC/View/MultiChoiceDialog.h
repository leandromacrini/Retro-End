/* MultiChoice Dialog class
*
* Create a Modal Dialog to let the user choice between the given values
* 
*/

#pragma once

#include "../../globals.h"

#include "BaseView.h"
#include "Label.h"
#include "ListView.h"

namespace RetroEnd
{
	namespace View
	{
		class MultiChoiceDialog : public BaseView
		{
		
		public:
			MultiChoiceDialog();
			
			void showDialog(string message, string title, vector<string>* values, function<void (unsigned int selectedIndex)> callback);
			void move(int direction); // 1 Next | -1 Prev

			bool input(Model::InputConfig* config, Model::Input input) override;
		private:
			vector<string>* mValues;
			function<void (unsigned int selectedIndex)> mCallback;

			BaseView* mBack;
			Label* mTitle;
			Label* mMessage;
			ListView* mValuesList;
		};
	}
}
