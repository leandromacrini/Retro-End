#pragma once

#include "BaseView.h"
#include "Image.h"
#include "../Model/Font.h"

/*


	ROW:

	|		|	TITLE	|		|
	| LEFT	|			| RIGHT	|
	| IMAGE	|			| IMAGE	|
	|		| SUBTITLE	|		|


*/

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

			unsigned int countRow();

			void setSelectedIndex(int index);
			unsigned int getSelectedIndex();

			//ROW ITEM COLORS

			unsigned int TitleColor;
			unsigned int SelectedTitleColor;

			unsigned int SubtitleColor;
			unsigned int SelectedSubtitleColor;

			unsigned int RowBackgroundColor;
			unsigned int SelectedRowBackgroundColor;

			unsigned int RowHeight;

			//FONTS

			std::shared_ptr<Model::Font> ItemFont;

			//SETTINGS
			TextAlign HorizontalTextAlign;

			//DRAW OVERRIDE

			void draw() override;
		private:
			vector<string> mItems;
			
			int mSelectedIndex;
		};
	}

}
