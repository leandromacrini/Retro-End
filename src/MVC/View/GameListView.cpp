#include "GameListView.h"

#include "../Controller/LogController.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

GameListView::GameListView() : BaseView()
{
	TitleColor = 0xFFFFFFFF;
	SelectedTitleColor = 0xFFFFFFFF;
	RowBackgroundColor = 0x00000000;
	SelectedRowBackgroundColor = 0x00000000;
	ItemFont = NULL;
	RowHeight = 50;
	mSelectedIndex = 0;
	HorizontalTextAlign = TextAlign::Left;
	firstRow = 0;
	lastRow = 0;

	mPointer = new Image();
	mPointer->Visible = false;
	this->addChild(mPointer);

	//create icons
	mTex1player = TextureResource::get("data/images/icon_player_1.png");
	mTex2player = TextureResource::get("data/images/icon_player_2.png");
	mTex3player = TextureResource::get("data/images/icon_player_3.png");
	mTex4player = TextureResource::get("data/images/icon_player_4.png");
	mTexCoOp    = TextureResource::get("data/images/icon_coop.png");
	mTexFavorite= TextureResource::get("data/images/icon_favorite.png");
}

// ROWS HANDLING
void GameListView::addRow(Game game, int index)
{
	if(index > -1)
		mItems.insert(mItems.begin() + index, game);
	else
		mItems.push_back(game);

	if(mSelectedIndex <= index && mSelectedIndex != 0)
	{
		mSelectedIndex++;
	}
}

void GameListView::removeRow(int index)
{
	mItems.erase(mItems.begin() + index);

	if(mSelectedIndex >= index && mSelectedIndex != 0)
	{
		mSelectedIndex--;
	}
}

void GameListView::removeAllRows()
{
	mItems.clear();
	mSelectedIndex = 0;
}

unsigned int GameListView::countRow()
{
	return mItems.size();
}

void GameListView::setSelectedIndex(int index)
{
	if(index >= (int) mItems.size())
		mSelectedIndex = 0;
	else if (index < 0)
		mSelectedIndex = mItems.size() - 1;
	else
		mSelectedIndex = index;
}

unsigned int GameListView::getSelectedIndex()
{
	return mSelectedIndex;
}

void GameListView::setPointerPath(string& path)
{
	mPointer->setPath(path);
}

void GameListView::draw()
{
	if( ! ItemFont ) ItemFont = Font::getDefaultFont();

	BaseView::draw();

	if(mItems.size() == 0) return;

	//compute the number of visible rows
	int visibleRows = (int) mSize.y() / RowHeight;

	//first and last visible row
	if(lastRow == 0)
		lastRow = firstRow + visibleRows - 1; 

	//if selected row is not complete visible move shown rows
	while(mSelectedIndex > lastRow)  { firstRow++; lastRow ++; }
	while(mSelectedIndex < firstRow) { firstRow--; lastRow --; }

	//hide pointer
	mPointer->Visible = false;

	//calculate current position and opacity only once
	Eigen::Vector3f absPosition = getAbsolutePosition();
	unsigned int absOpacity = getAbsoluteOpacity();

	for(int i = 0; i < (int)mItems.size(); i++)
	{
		//draw only visible rows (partial rows too -> from firstRow-1 TO lastRow+1)
		if(firstRow != 0 && i < firstRow-1) continue;
		if(i > lastRow +1) continue;

		float rowYPosition = (float) i - firstRow;

		//compute the text align offset
		int xTextOffset = 0; //MUST BE AN INTEGER
		switch (HorizontalTextAlign)
		{
		case RetroEnd::View::Left:
			xTextOffset = (int)(absPosition.x() + RowHeight + 30);
			break;
		case RetroEnd::View::Center:
			xTextOffset = (int)(absPosition.x() + ( ( mSize.x() - ItemFont->sizeText(mItems.at(i).Title).x() ) / 2));
			break;
		case RetroEnd::View::Right:
			xTextOffset = (int)(absPosition.x() + mSize.x() - ItemFont->sizeText(mItems.at(i).Title).x());
			break;
		default:
			break;
		}

		//draw pointer
		if(mSelectedIndex == i)
		{
			//draw background
			RenderController::drawRect(
				(int)absPosition.x() + RowHeight + 15, (int)(absPosition.y() + rowYPosition*RowHeight), (int)mSize.x(), RowHeight,
				SelectedRowBackgroundColor
			);

			mPointer->setSize((float)RowHeight,(float)RowHeight);
			mPointer->Visible = true;
			mPointer->setPosition(0, RowHeight * rowYPosition);
		}
		else
		{
			//draw background
			RenderController::drawRect(
				(int)absPosition.x(), (int)(absPosition.y() + rowYPosition*RowHeight), (int)mSize.x(), RowHeight,
				RowBackgroundColor
			);
		}

		//draw icons

		int maxPosition = 0;

		//icon at position 1
		if(mItems.at(i).Favorite)
		{
			drawGameIcon( mTexFavorite, absPosition.x() + mSize.x() - RowHeight, getAbsolutePosition().y() + (rowYPosition)*RowHeight, RowHeight,  i == mSelectedIndex? 0xFFFFFFFF : 0xFFFFFF66);
			maxPosition=1;
		}
		//icon at position 2
		if(mItems.at(i).MaxPlayers == "1")
		{
			drawGameIcon( mTex1player, absPosition.x() + mSize.x() - RowHeight*2, getAbsolutePosition().y() + (rowYPosition)*RowHeight, RowHeight,  i == mSelectedIndex? 0xFFFFFFFF : 0xFFFFFF66);
			maxPosition=2;
		}
		else if(mItems.at(i).MaxPlayers == "2")
		{
			drawGameIcon( mTex2player, absPosition.x() + mSize.x() - RowHeight*2, getAbsolutePosition().y() + (rowYPosition)*RowHeight, RowHeight,  i == mSelectedIndex? 0xFFFFFFFF : 0xFFFFFF66);
			maxPosition=2;
		}
		else if(mItems.at(i).MaxPlayers == "3")
		{
			drawGameIcon( mTex3player, absPosition.x() + mSize.x() - RowHeight*2, getAbsolutePosition().y() + (rowYPosition)*RowHeight, RowHeight,  i == mSelectedIndex? 0xFFFFFFFF : 0xFFFFFF66);
			maxPosition=2;
		}
		else if(mItems.at(i).MaxPlayers == "4+")
		{
			drawGameIcon( mTex4player, absPosition.x() + mSize.x() - RowHeight*2, getAbsolutePosition().y() + (rowYPosition)*RowHeight, RowHeight,  i == mSelectedIndex? 0xFFFFFFFF : 0xFFFFFF66);
			maxPosition=2;
		}
		//icon at position 3
		if(mItems.at(i).CoOp == "Yes")
		{
			drawGameIcon( mTexCoOp, absPosition.x() + mSize.x() - RowHeight*3, getAbsolutePosition().y() + (rowYPosition)*RowHeight, RowHeight,  i == mSelectedIndex? 0xFFFFFFFF : 0xFFFFFF66);
			maxPosition=3;
		}

		//draw text
		ItemFont->drawText(
			mItems.at(i).Title,
			Eigen::Vector2f( xTextOffset, absPosition.y() + (rowYPosition)*RowHeight + (RowHeight - ItemFont->sizeText(mItems.at(i).Title).y())/2 ),
			i == mSelectedIndex? SelectedTitleColor : TitleColor,
			(int)(-(xTextOffset - absPosition.x()) + mSize.x() - RowHeight*maxPosition) // size - pointer - icons
		);
	}
}

void GameListView::drawGameIcon( shared_ptr<Model::TextureResource> texture, float posX, float posY, int side, unsigned int opacity)
{
	GLubyte colors[6*4];
	GLfloat points[12], texs[12];
	RenderController::buildGLColorArray(colors, opacity, 6);

	points[0]  = posX;				points[1]  = posY;
	points[2]  = posX;				points[3]  = posY + side;
	points[4]  = posX + side;		points[5]  = posY;

	points[6]  = posX + side;		points[7]  = posY;
	points[8]  = posX;				points[9]  = posY + side;
	points[10] = posX + side;		points[11] = posY + side;



	texs[0] = 0;		texs[1] = 1;
	texs[2] = 0;		texs[3] = 0;
	texs[4] = 1;		texs[5] = 1;

	texs[6] = 1;		texs[7] = 1;
	texs[8] = 0;		texs[9] = 0;
	texs[10] = 1;		texs[11] = 0;

	texture->bind();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	if(colors != NULL)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors);
	}

	glVertexPointer(2, GL_FLOAT, 0, points);
	glTexCoordPointer(2, GL_FLOAT, 0, texs);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	if(colors != NULL)
		glDisableClientState(GL_COLOR_ARRAY);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}