#pragma once

#include "../../globals.h"

#include "../Model/Observer.h"
#include "../Model/Font.h"

#include "BaseView.h"
#include "Image.h"

#define MOVE_DELAY 150

namespace RetroEnd
{
	namespace View
	{
		class OptionsListView : public BaseView
		{
		public:
			OptionsListView();

			// ROWS HANDLING
			void addRow(string description, BaseView* child, int index = -1);

			void removeRow(int index = 0);

			void removeAllRows();

			void setAllRows(vector<pair<string, BaseView*>>* values);

			unsigned int countRow();

			void setSelectedIndex(int index);
			unsigned int getSelectedIndex();

			//OVERRIDES
			void draw() override;
			bool input(Model::Input input) override;
			void update(unsigned int deltaTime) override;

			//EVENTS
			Model::Observer<unsigned int> onItemPressed;
			Model::Observer<int> onSelectedItemChanged;
			Model::Observer<int> onFastMoved;

		private:
			unsigned int mTitleColor;
			unsigned int mSelectedTitleColor;

			unsigned int mRowBackgroundColor;
			unsigned int mSelectedRowBackgroundColor;

			unsigned int mRowHeight;

			int mMoving; // 1 Next | 0 Stop | -1 Prev
			clock_t mLastCheck;
			clock_t mStartedMoving;
			void startMoving(int direction); // 1 Next | -1 Prev
			void stopMoving();

			int firstRow;
			int lastRow;
			vector<pair<string, BaseView*>> mItems;
			
			int mSelectedIndex;
		};
	}

}
