#include "MessageDialog.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

MessageDialog::MessageDialog() : BaseView()
{

	float W = (float) RenderController::getInstance().getScreenWidth();
	float H = (float) RenderController::getInstance().getScreenHeight();

	mSize = Eigen::Vector2f(W,H);
	BackgroundColor = 0x1d1d1d99;

	BaseView* mBack = new BaseView();
	mBack->BackgroundColor = 0xEDEDEDFF;
	mBack->setSize(W * 5/10, H * 4/10);
	mBack->setPosition(W* 2.5f/10, H * 3/10);
	addChild(mBack);

	mMessage = new Label();
	mMessage->setSize(mBack->getSize());
	mMessage->setPosition(0, (mBack->getSize().y() - mMessage->getFont()->getSize())/2);
	mMessage->HorizontalTextAlign = TextAlign::Center;
	mMessage->WrapText = true;
	mBack->addChild(mMessage);
}

void MessageDialog::setMessage(string message)
{
	mMessage->setText(message);
}
