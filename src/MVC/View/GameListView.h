#pragma once

#include "BaseView.h"
#include "Image.h"

#include "../Model/Font.h"
#include "../Model/Game.h"
#include "../Model/TextureResource.h"

namespace RetroEnd
{
	namespace View
	{
		class GameListView : public BaseView
		{
		public:
			GameListView();

			// ROWS HANDLING
			void addRow(Model::Game game, int index = -1);

			void removeRow(int index = 0);

			void removeAllRows();

			unsigned int countRow();

			void setSelectedIndex(int index);
			unsigned int getSelectedIndex();

			//POINTER
			void setPointerPath(string& path);

			//ROW ITEM COLORS
			unsigned int TitleColor;
			unsigned int SelectedTitleColor;

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
			int firstRow;
			int lastRow;
			int mSelectedIndex;

			vector<Model::Game> mItems;
			Image* mPointer;

			shared_ptr<Model::TextureResource> mTex1player;
			shared_ptr<Model::TextureResource> mTex2player;
			shared_ptr<Model::TextureResource> mTex3player;
			shared_ptr<Model::TextureResource> mTex4player;
			shared_ptr<Model::TextureResource> mTexCoOp;
			shared_ptr<Model::TextureResource> mTexFavorite;

			void drawGameIcon( shared_ptr<Model::TextureResource> texture, float posX, float posY, int side, unsigned int opacity);
		};
	}

}
