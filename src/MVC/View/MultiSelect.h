#pragma once

#include "BaseView.h"

#include "MultiChoiceDialog.h"

#include "../Model/Font.h"
#include "../Model/Observer.h"

namespace RetroEnd
{
	namespace View
	{
		class MultiSelect : public BaseView
		{
		public:
			MultiSelect(string message, string title, vector<string>* values, int selected = 0);

			//EVENTS

			//Selected item changed event
			Model::Observer<int> onSelectedItemChanged;

			//OVERRIDES

			void draw() override;
			bool input(Model::Input input) override;

		private:
			string mText;
			string mMessage;
			string mTitle;
			vector<string>* mValues;
			int mSelectedIndex;

			MultiChoiceDialog* mDialog;
		};
	}

}
