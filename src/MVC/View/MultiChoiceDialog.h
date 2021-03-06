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

#include "../Controller/LogController.h"

namespace RetroEnd
{
	namespace View
	{
		class MultiChoiceDialog : public BaseView
		{
		
		public:
			MultiChoiceDialog();
			
			void showDialog(string message, string title, vector<string>* values, function<void (unsigned int selectedIndex)> callback, int selected = 0);

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
