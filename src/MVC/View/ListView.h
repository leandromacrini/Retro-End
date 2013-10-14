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
		class ListViewItem
		{
		public:
			ListViewItem() : title(NULL), subtitle(NULL), leftIMage(NULL), rightIMage(NULL), childView(NULL) { }

			const char* title;
			const char* subtitle;
			Image* leftIMage;
			Image* rightIMage;
			BaseView* childView;
		};

		class ListView : public BaseView
		{
		public:
			ListView();

			// ROWS HANDLING
			void addRow(ListViewItem& row, int index = -1);
			void addRow(BaseView* row, int index = -1);
			void addRow(const char* title, int index = -1);

			void removeRow(unsigned int index = 0);

			unsigned int countRow();

			//ROW ITEM COLORS

			unsigned int TitleColor;
			unsigned int SelectedTitleColor;

			unsigned int SubtitleColor;
			unsigned int SelectedSubtitleColor;

			unsigned int RowBackgroundColor;
			unsigned int SelectedRowBackgroundColor;

			unsigned int RowHeight;

			//FONTS

			void setFont(std::shared_ptr<Model::Font> font);
			std::shared_ptr<Model::Font> getFont() const;

			//DRAW OVERRIDE

			void draw() override;
		private:
			vector<ListViewItem> mItems;
			std::shared_ptr<Model::Font> mFont;

			int mSelectedRow;
		};
	}

}
