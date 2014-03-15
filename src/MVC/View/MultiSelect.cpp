#include "MultiSelect.h"

#include "../Controller/LogController.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

MultiSelect::MultiSelect(string message, string title, vector<string>* values, int selected) : BaseView(), 
	mMessage(message),
	mTitle(title),
	mValues(values),
	mSelectedIndex(selected),
	mDialog(NULL)
{
	mText = mValues->at(mSelectedIndex);
}

bool MultiSelect::input(Input input)
{
	if(mDialog)
	{
		return mDialog->input(input);
	}

	if(input.Semantic == BUTTON_A && input.Value == SDL_PRESSED )
	{
		mDialog = new MultiChoiceDialog();
		RenderController::getInstance().getCurrentWindow()->addChild(mDialog);
		mDialog->showDialog(mMessage, mTitle, mValues, [this] (unsigned int index)
		{
			mText = mValues->at(index);
			onSelectedItemChanged(index);
			RenderController::getInstance().getCurrentWindow()->removeChild(mDialog);
			mDialog = NULL;
		},
		mSelectedIndex);
		return true;
	}

	return false;
}

void MultiSelect::draw()
{
	//draw the parent
	BaseView::draw();

	//the draw me
	//with opacity *= parent opacity (recursively)
	if(getOpacity() > 0 && Visible)
	{
		//draw title
		if( ! mText.empty() )
		{
			Eigen::Vector2f tSize = Font::getDefaultFont()->sizeText(mText);
			Eigen::Vector3f absPos = getAbsolutePosition();

			Font::getDefaultFont()->drawText(mText,
				Eigen::Vector2f(
					(int)max(absPos.x(), (absPos.x() + ( ( mSize.x() - tSize.x() ) / 2))),
					(int)(absPos.y() + ( ( mSize.y() - tSize.y() ) / 2))
				),
				0x000000FF >> 8 << 8  | getAbsoluteOpacity(), (int)mSize.x());
		}
	}
}
