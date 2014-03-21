#include "ListView.h"

#include "../Controller/LogController.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

ListView::ListView() : BaseView(), mFont(NULL), mMoving(0), mLastCheck(0), mStartedMoving(0)
{
	TitleColor = 0xFFFFFFFF;
	SelectedTitleColor = 0xFFFFFFFF;
	SubtitleColor = 0xFFFFFFFF;
	SelectedSubtitleColor = 0xFFFFFFFF;
	RowBackgroundColor = 0x00000000;
	SelectedRowBackgroundColor = 0x00000000;
	RowHeight = 50;
	mSelectedIndex = 0;
	HorizontalTextAlign = TextAlign::Left;
	Focusable = true;

	firstRow = 0;
	lastRow = 0;
}

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

void ListView::setAllRows(vector<string>* values)
{
	mItems.clear();
	mSelectedIndex = 0;

	for(Uint32 i = 0; values && i < values->size(); i++)
	{
		mItems.push_back(values->at(i));
	}
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

string ListView::getSelectedTitle()
{
	if((int)mItems.size() > mSelectedIndex)
		return mItems[mSelectedIndex];
	else
		return NULL;
}

void ListView::setFont(std::shared_ptr<Font> font)
{
	mFont = font;
}

std::shared_ptr<Font> ListView::getFont() const
{
	if(mFont)
		return mFont;
	else
		return Font::getDefaultFont();
}

void ListView::draw()
{
	BaseView::draw();

	if( ! mFont ) mFont = getFont();

	Eigen::Vector3f absPos = getAbsolutePosition();

	if(mItems.size() == 0)
	{
		//draw text
		mFont->drawText("None", Eigen::Vector2f(
			(int)(absPos.x() + ( ( mSize.x() - mFont->sizeText("None").x()) / 2)),
			(int)(absPos.y() + ( ( mSize.y() - mFont->sizeText("None").y()) / 2))),
			TitleColor);
	}
	else
	{
		//compute the number of visible rows
		int visibleRows = (int) mSize.y() / RowHeight;

		//first and last visible row
		if(lastRow == 0)
			lastRow = firstRow + visibleRows - 1; 

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
			int xTextOffset = 0;
			switch (HorizontalTextAlign)
			{
			case Left:
				xTextOffset = (int)absPos.x();
				break;
			case Center:
				xTextOffset = (int)max(absPos.x(),(absPos.x() + ( ( mSize.x() - mFont->sizeText(mItems.at(i)).x() ) / 2)));
				break;
			case Right:
				xTextOffset = (int)(absPos.x() + mSize.x() - mFont->sizeText(mItems.at(i)).x());
				break;
			default:
				break;
			}

			//draw pointer
			if(Focused)
			{
				if(mSelectedIndex == i)
				{
					RenderController::drawRect(
						(int)absPos.x(), (int)(absPos.y() + rowYPosition*RowHeight), (int)mSize.x(), RowHeight,
						SelectedRowBackgroundColor
					);
				}
				else
				{
					//draw background
					RenderController::drawRect(
						(int)absPos.x(), (int)(absPos.y() + rowYPosition*RowHeight), (int)mSize.x(), RowHeight,
						RowBackgroundColor
					);
				}
			}
		
			//draw text
			mFont->drawText(mItems.at(i), Eigen::Vector2f(
				xTextOffset,
				absPos.y() + (rowYPosition)*RowHeight + (RowHeight - mFont->sizeText(mItems.at(i)).y())/2),
				i == mSelectedIndex && Focused? SelectedTitleColor : TitleColor, (int)mSize.x());
		}
	}
}


void ListView::startMoving(int direction)
{
	mLastCheck = clock();
	mStartedMoving = clock();
	mMoving = direction;
}

void ListView::stopMoving()
{
	mMoving = 0;
	clock_t now = clock();
	if(mStartedMoving && now - mStartedMoving > MOVE_DELAY)
	{
		onSelectedItemChanged(mSelectedIndex);
		mStartedMoving = 0;
	}
}

bool ListView::input(Input input)
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

	if(Focused && input.Semantic == InputSemantic::LEFT && input.Value != SDL_RELEASED )
	{
		setSelectedIndex(- (int)min(10u, getSelectedIndex()));
		return true;
	}

	if(Focused && input.Semantic == InputSemantic::RIGHT && input.Value != SDL_RELEASED )
	{
		setSelectedIndex(min(10u, mItems.size() -1 - getSelectedIndex()));
		return true;
	}

	if(Focused && input.Semantic == InputSemantic::BUTTON_A && input.Value != SDL_RELEASED )
	{
		onItemPressed(mSelectedIndex);
		return true;
	}

	return false;
}

void ListView::update(unsigned int deltaTime)
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