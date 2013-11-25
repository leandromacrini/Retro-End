#include "ListView.h"

#include "../Controller/LogController.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

ListView::ListView() : BaseView()
{
	TitleColor = 0xFFFFFFFF;
	SelectedTitleColor = 0xFFFFFFFF;
	SubtitleColor = 0xFFFFFFFF;
	SelectedSubtitleColor = 0xFFFFFFFF;
	RowBackgroundColor = 0x00000000;
	SelectedRowBackgroundColor = 0x00000000;
	ItemFont = NULL;
	RowHeight = 50;
	mSelectedIndex = 0;
	HorizontalTextAlign = TextAlign::Left;

	firstRow = 0;
	lastRow = 0;
}

// ROWS HANDLING
void ListView::addRow(string title, int index)
{
	if(index > -1)
		mItems.insert(mItems.begin() + index, title);
	else
		mItems.push_back(title);

	if(mSelectedIndex <= index && mSelectedIndex != 0)
	{
		mSelectedIndex++;
	}
}


void ListView::removeRow(int index)
{
	mItems.erase(mItems.begin() + index);

	if(mSelectedIndex >= index && mSelectedIndex != 0)
	{
		mSelectedIndex--;
	}
}

void ListView::removeAllRows()
{
	mItems.clear();
	mSelectedIndex = 0;
}

unsigned int ListView::countRow()
{
	return mItems.size();
}

void ListView::setSelectedIndex(int index)
{
	if(index >= (int) mItems.size())
		mSelectedIndex = 0;
	else if (index < 0)
		mSelectedIndex = mItems.size() - 1;
	else
		mSelectedIndex = index;
}

unsigned int ListView::getSelectedIndex()
{
	return mSelectedIndex;
}

void ListView::draw()
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

	for(int i = 0; i < (int)mItems.size(); i++)
	{
		//draw only visible rows (partial rows too -> from firstRow-1 TO lastRow+1)
		if(firstRow != 0 && i < firstRow-1) continue;
		if(i > lastRow +1) continue;

		float rowYPosition = (float) i - firstRow;

		//compute the text align offset
		int xTextOffset = 0;
		switch (HorizontalTextAlign)
		{
		case RetroEnd::View::Left:
			xTextOffset = (int)getAbsolutePosition().x();
			break;
		case RetroEnd::View::Center:
			xTextOffset = (int)(getAbsolutePosition().x() + ( ( mSize.x() - ItemFont->sizeText(mItems.at(i)).x() ) / 2));
			break;
		case RetroEnd::View::Right:
			xTextOffset = (int)(getAbsolutePosition().x() + mSize.x() - ItemFont->sizeText(mItems.at(i)).x());
			break;
		default:
			break;
		}

		//draw pointer
		if(mSelectedIndex == i)
		{
			//draw background
			RenderController::drawRect(
				(int)getAbsolutePosition().x(), (int)(getAbsolutePosition().y() + rowYPosition*RowHeight), (int)mSize.x(), RowHeight,
				SelectedRowBackgroundColor
			);
		}
		else
		{
			//draw background
			RenderController::drawRect(
				(int)getAbsolutePosition().x(), (int)(getAbsolutePosition().y() + rowYPosition*RowHeight), (int)mSize.x(), RowHeight,
				RowBackgroundColor
			);
		}
		
		//draw text
		ItemFont->drawText(mItems.at(i), Eigen::Vector2f(
			xTextOffset,
			getAbsolutePosition().y() + (rowYPosition)*RowHeight + (RowHeight - ItemFont->sizeText(mItems.at(i)).y())/2),
			i == mSelectedIndex? SelectedTitleColor : TitleColor);
	}
}