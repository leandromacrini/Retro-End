#include "OptionsListView.h"

#include "../Controller/LogController.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

OptionsListView::OptionsListView() : BaseView(), mMoving(0), mLastCheck(0), mStartedMoving(0)
{
	mTitleColor = 0x000000FF;
	mSelectedTitleColor = 0xFFFFFFFF;
	mRowBackgroundColor = 0x00000000;
	mSelectedRowBackgroundColor = 0x1d1d1d99;
	mRowHeight = 50;
	mSelectedIndex = 0;
	Focusable = true;

	firstRow = 0;
	lastRow = 0;
}

void OptionsListView::addRow(string description, BaseView* child, int index)
{
	if(index > -1)
		mItems.insert(mItems.begin() + index, pair<string, BaseView*>(description, child));
	else
		mItems.push_back(pair<string, BaseView*>(description, child));

	//update id pushed before current selected item
	if(mSelectedIndex <= index && mSelectedIndex != 0)
	{
		setSelectedIndex(mSelectedIndex + 1);
	}

	//select the item if size is one
	if(mItems.size() == 1) setSelectedIndex(0);
}

void OptionsListView::removeRow(int index)
{
	mItems.erase(mItems.begin() + index);

	if(mSelectedIndex >= index && mSelectedIndex != 0)
	{
		setSelectedIndex(mSelectedIndex - 1);
	}
}

void OptionsListView::removeAllRows()
{
	mItems.clear();
	mSelectedIndex = 0;
}

void OptionsListView::setAllRows(vector<pair<string, BaseView*>>* values)
{
	mItems.clear();

	for(Uint32 i = 0; values && i < values->size(); i++)
	{
		mItems.push_back(values->at(i));
	}

	setSelectedIndex(0);
}

unsigned int OptionsListView::countRow()
{
	return mItems.size();
}

void OptionsListView::setSelectedIndex(int index)
{
	//unfocus prev selected row
	if(mItems[mSelectedIndex].second && mItems[mSelectedIndex].second->Focusable) mItems[mSelectedIndex].second->Focused = false;

	if(index >= (int) mItems.size())
		mSelectedIndex = 0;
	else if (index < 0)
		mSelectedIndex = mItems.size() - 1;
	else
		mSelectedIndex = index;

	//focus new selected row
	if(mItems[mSelectedIndex].second && mItems[mSelectedIndex].second->Focusable) mItems[mSelectedIndex].second->Focused = true;
}

unsigned int OptionsListView::getSelectedIndex()
{
	return mSelectedIndex;
}

void OptionsListView::draw()
{
	BaseView::draw();

	shared_ptr<Font> font = Font::getDefaultFont();

	Eigen::Vector3f absPos = getAbsolutePosition();

	if(mItems.size() == 0)
	{
		//draw text
		font->drawText("None", Eigen::Vector2f(
			(int)(absPos.x() + ( ( mSize.x() - font->sizeText("None").x()) / 2)),
			(int)(absPos.y() + ( ( mSize.y() - font->sizeText("None").y()) / 2))),
			mTitleColor);
	}
	else
	{
		//compute the number of visible rows
		int visibleRows = (int) mSize.y() / mRowHeight;

		//first and last visible row
		if(lastRow == 0) lastRow = firstRow + visibleRows - 1; 

		//if selected row is not complete visible move shown rows
		while(mSelectedIndex > lastRow)  { firstRow++; lastRow ++; }
		while(mSelectedIndex < firstRow) { firstRow--; lastRow --; }

		for(Uint32 i = 0; i < mItems.size(); i++)
		{
			//draw only visible rows (partial rows too -> from firstRow-1 TO lastRow+1)
			if(firstRow != 0 && (int)i < firstRow-1) continue;
			if((int)i > lastRow +1) continue;

			float rowYPosition = (float) i - firstRow;

			//compute the text align offset
			int xTextOffset = (int)absPos.x();

			//draw background
			if(mSelectedIndex == i)
			{
				Uint32 selectedBack = mSelectedRowBackgroundColor;
				//make SelectedTitleColor more transparent if list is not focused
				if (! Focused ) selectedBack = (mSelectedRowBackgroundColor>>8<<8) | 0x00000055;

				RenderController::drawRect( (int)absPos.x(), (int)(absPos.y() + rowYPosition*mRowHeight), (int)mSize.x(), mRowHeight, selectedBack );
			}
			else
			{
				RenderController::drawRect( (int)absPos.x(), (int)(absPos.y() + rowYPosition*mRowHeight), (int)mSize.x(), mRowHeight, mRowBackgroundColor );
			}
		
			//draw text
			font->drawText(mItems.at(i).first, Eigen::Vector2f(
				xTextOffset,
				absPos.y() + (rowYPosition)*mRowHeight + (mRowHeight - font->sizeText(mItems.at(i).first).y())/2),
				i == mSelectedIndex? mSelectedTitleColor : mTitleColor, (int)mSize.x());

			//draw component
			BaseView* control = mItems.at(i).second;
			if(control)
			{
				control->setPosition(absPos.x() + mSize.x() - control->getSize().x(), (absPos.y() + rowYPosition*mRowHeight));
				control->draw();
			}
		}
	}
}


void OptionsListView::startMoving(int direction)
{
	mLastCheck = clock();
	mStartedMoving = clock();
	mMoving = direction;
}

void OptionsListView::stopMoving()
{
	mMoving = 0;
	clock_t now = clock();
	if(mStartedMoving && now - mStartedMoving > MOVE_DELAY)
	{
		onSelectedItemChanged(mSelectedIndex);
		mStartedMoving = 0;
	}
}

bool OptionsListView::input(Input input)
{
	if(Focused && input.Semantic == InputSemantic::DOWN)
	{
		if( input.Value == SDL_PRESSED )
		{
			//down
			setSelectedIndex( getSelectedIndex() + 1 );
			startMoving(1);
		}
		else
		{
			//up
			stopMoving();
		}
		return true;
	}

	if(Focused && input.Semantic == InputSemantic::UP)
	{
		if( input.Value == SDL_PRESSED )
		{
			//down
			setSelectedIndex( getSelectedIndex() - 1 );
			startMoving(-1);
		}
		else
		{
			//up
			stopMoving();
		}
		return true;
	}

	if(mMoving) return false;

	//send input to control of selected row (is any)
	if(mItems.at(mSelectedIndex).second && mItems.at(mSelectedIndex).second->input(input))
	{
		return true;
	}

	//only if input control of selected row ignore button press we send the event
	if(Focused && input.Semantic == InputSemantic::BUTTON_A && input.Value != SDL_RELEASED )
	{
		onItemPressed(mSelectedIndex);
		return true;
	}

	return false;
}

void OptionsListView::update(unsigned int deltaTime)
{
	//call father method
	BaseView::update(deltaTime);

	//check if we need to move the list for long pressure
	clock_t now = clock();
	if(Focused)
	{
		if(mMoving != 0 && now - mLastCheck > MOVE_DELAY)
		{
			setSelectedIndex( getSelectedIndex() + mMoving);
			mLastCheck = now;
			onFastMoved(mSelectedIndex);
		}
	}
	else
	{
		//if focus change while we are moving stop moving
		if(mMoving) stopMoving();
	}
}