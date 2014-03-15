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
		class ListView : public BaseView
		{
		public:
			ListView();

			// ROWS HANDLING
			void addRow(string title, int index = -1);

			void removeRow(int index = 0);

			void removeAllRows();

			void setAllRows(vector<string>* values);

			unsigned int countRow();

			void setSelectedIndex(int index);
			unsigned int getSelectedIndex();
			string getSelectedTitle();

			//ROW ITEM COLORS

			unsigned int TitleColor;
			unsigned int SelectedTitleColor;

			unsigned int SubtitleColor;
			unsigned int SelectedSubtitleColor;

			unsigned int RowBackgroundColor;
			unsigned int SelectedRowBackgroundColor;

			unsigned int RowHeight;

			//FONTS

			shared_ptr<Model::Font> getFont() const;
			void setFont(shared_ptr<Model::Font>);

			//SETTINGS
			TextAlign HorizontalTextAlign;

			//OVERRIDES
			void draw() override;
			bool input(Model::Input input) override;
			void update(unsigned int deltaTime) override;

			//EVENTS
			Model::Observer<unsigned int> onItemPressed;
			Model::Observer<int> onSelectedItemChanged;
			Model::Observer<int> onFastMoved;

		private:
			std::shared_ptr<Model::Font> mFont;

			int mMoving; // 1 Next | 0 Stop | -1 Prev
			clock_t mLastCheck;
			clock_t mStartedMoving;
			void startMoving(int direction); // 1 Next | -1 Prev
			void stopMoving();

			int firstRow;
			int lastRow;
			vector<string> mItems;
			
			int mSelectedIndex;
		};
	}

}
