#pragma once

#include "BaseView.h"

#include "Image.h"

#include "../Model/Game.h"
#include "../Model/Font.h"
#include "../Model/Observer.h"
#include "../Model/Sound.h"
#include "../Model/TextureResource.h"

#define MOVE_DELAY 150

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
			void setPointerPath(string path);

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

			//OVERRIDES
			void draw() override;
			bool input(Model::Input input) override;
			void update(unsigned int deltaTime) override;

			//EVENTS
			Model::Observer<Model::Game> onItemPressed;
			Model::Observer<int> onSelectedItemChanged;
			Model::Observer<int> onFastMoved;
		private:
			shared_ptr<Model::Sound> mMoveSound;
			shared_ptr<Model::Sound> mSelectSound;

			int mMoving; // 1 Next | 0 Stop | -1 Prev
			clock_t mLastCheck;
			clock_t mStartedMoving;
			void startMoving(int direction); // 1 Next | -1 Prev
			void stopMoving();

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
