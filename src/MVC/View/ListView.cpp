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
	SelectedRowBackgroundColor = 0xFF0000FF;
	mFont = NULL;
	RowHeight = 50;
	mSelectedRow = 0;
}

// ROWS HANDLING
void ListView::addRow(ListViewItem& row, int index)
{
	if(index > -1)
		mItems.insert(mItems.begin() + index, row);
	else
		mItems.push_back(row);
}
void ListView::addRow(BaseView* row, int index)
{
	ListViewItem& item = ListViewItem();
	item.childView = row;

	addRow(item, index);

}
void ListView::addRow(const char* title, int index)
{
	ListViewItem& item = ListViewItem();
	item.title = title;

	addRow(item, index);
}

void ListView::removeRow(unsigned int index)
{
	mItems.erase(mItems.begin() + index);
}

unsigned int ListView::countRow()
{
	return mItems.size();
}

void ListView::setFont(shared_ptr<Font> font)
{
	mFont = font;
}
shared_ptr<Font> ListView::getFont() const
{
	if(mFont)
		return mFont;
	else
		return Font::getDefaultFont();
}

void ListView::draw()
{
	BaseView::draw();

	for(unsigned int i = 0; i < mItems.size(); i++)
	{
		ListViewItem& item = mItems.at(i);

		getFont()->drawCenteredText(item.title, 0, getAbsolutePosition().y() + i*RowHeight, TitleColor);
	}
}