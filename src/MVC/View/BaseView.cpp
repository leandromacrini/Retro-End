#include "BaseView.h"

#include "../Controller/RenderController.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

BaseView::BaseView() : 
	mParent(NULL),
	mOpacity(255),
	mPosition(Eigen::Vector3f::Zero()),
	mSize(Eigen::Vector2f(1,1)), //SIZE MUST NEVER BE ZERO WHEN RENDERED
	mAnimation(NULL),
	mBackgroundColor(0x00000000),
	mSelectedBackgroundColor(0x00000000),
	mSelected(false)
{

	Visible = true;
}

BaseView::~BaseView()
{
	removeAllChildren();
}

void BaseView::update(unsigned int deltaTime)
{
	bool delayed = false;
	//before start animation check if it need a delay
	if(mAnimation && mAnimation->millisDelay > 0)
	{
		delayed = true;
		mAnimation->millisDelay-= deltaTime;
	}

	//animation handling
	if( !delayed && mAnimation)
	{
		//Moving
		if(mAnimation->moveOffset)
		{
			float deltaX = mAnimation->moveOffset->x() * deltaTime / ( mAnimation->millisDuration > 0? mAnimation->millisDuration : 1 );
			float deltaY = mAnimation->moveOffset->y() * deltaTime / ( mAnimation->millisDuration > 0? mAnimation->millisDuration : 1 );

			//last time?
			if( abs(deltaX) > abs(mAnimation->moveOffset->x())) deltaX = mAnimation->moveOffset->x();
			if( abs(deltaY) > abs(mAnimation->moveOffset->y())) deltaY = mAnimation->moveOffset->y();

			*(mAnimation->moveOffset) -= Eigen::Vector3f(deltaX, deltaY, 0);
			mPosition += Eigen::Vector3f(deltaX, deltaY, 0);
		}

		//fading
		if(mAnimation->newOpacity && *mAnimation->newOpacity != mOpacity)
		{
			if(mAnimation->millisDuration == 0)
			{
				mOpacity =  *mAnimation->newOpacity;
			}
			else 
			{
				int deltaOpacity = (mOpacity - *mAnimation->newOpacity ) * (int)deltaTime / (int)( mAnimation->millisDuration > 0? mAnimation->millisDuration : 1 );

				if(abs(deltaOpacity) > abs( mOpacity - *mAnimation->newOpacity ))
					mOpacity = *mAnimation->newOpacity;
				else
					mOpacity -= deltaOpacity;
			}
		}

		//scaling
		if(mAnimation->newSize && *mAnimation->newSize != mSize)
		{
			if(mAnimation->millisDuration == 0)
			{
				mSize =  *mAnimation->newSize;
			}
			else
			{
				float deltaW = (mSize.x() - mAnimation->newSize->x()) * deltaTime / mAnimation->millisDuration;
				float deltaH = (mSize.y() - mAnimation->newSize->y()) * deltaTime / mAnimation->millisDuration;

				mSize -= Eigen::Vector2f(deltaW, deltaH);
			}
		}

		//is animation completed?
		if(mAnimation->millisDuration == 0)
		{
			mAnimation->completed = true;

			//save callback
			auto callback = mAnimation->endCallback;

			//delete current animation so we can chain another animation in the callback
			delete mAnimation;
			mAnimation = NULL;

			callback();
		}
		else
		{
			mAnimation->millisDuration -= deltaTime >= mAnimation->millisDuration ? mAnimation->millisDuration : deltaTime ;
		}
	}

	for(unsigned int i = 0; i < getChildCount(); i++)
	{
		getChild(i)->update(deltaTime);
	}
}

Eigen::Vector3f BaseView::getPosition() const
{
	return mPosition;
}

void BaseView::setPosition(const Eigen::Vector3f& offset)
{
	mPosition = offset;
	onPositionChanged();
}

void BaseView::setPosition(float x, float y, float z)
{
	mPosition << x, y, z;
	onPositionChanged();
}

Eigen::Vector2f BaseView::getSize() const
{
	return mSize;
}

void BaseView::setSize(const Eigen::Vector2f& size)
{
	mSize = size;
	onSizeChanged();
}

void BaseView::setSize(float w, float h)
{
	mSize << w, h;
	onSizeChanged();
}

unsigned char BaseView::getOpacity() const
{
	return mOpacity;
}

void BaseView::setOpacity(unsigned char opacity)
{
	mOpacity = opacity;
}

unsigned int BaseView::getBackgroundColor() const
{
	return mBackgroundColor;
}

void BaseView::setBackgroundColor(unsigned int color)
{
	mBackgroundColor = color;
}

unsigned int BaseView::getSelectedBackgroundColor() const
{
	return mSelectedBackgroundColor;
}

void BaseView::setSelectedBackgroundColor(unsigned int color)
{
	mSelectedBackgroundColor = color;
}
void BaseView::setSelected(bool selected)
{
	mSelected = selected;
}
bool BaseView::getSelected() const
{
	return mSelected;
}

const Eigen::Affine3f BaseView::getTransform()
{
	mTransform.setIdentity();
	return mTransform;
}

void BaseView::addChild(BaseView* cmp)
{
	mChildren.push_back(cmp);

	if(cmp->getParent())
		cmp->getParent()->removeChild(cmp);

	cmp->setParent(this);
}

//this function remove the child from the vector and call delete on it
void BaseView::removeChild(BaseView* cmp)
{
	vector<BaseView*>::iterator it = find(mChildren.begin(), mChildren.end(), cmp);
	if(it != mChildren.end())
	{
		mChildren.erase(it);
		//delete cmp; //TODO Check if many errors from "update" come from this
	}
}

void BaseView::removeAllChildren()
{
	while(mChildren.size() > 0)
	{
		removeChild(mChildren.at(0));
	}
}

unsigned int BaseView::getChildCount() const
{
	return mChildren.size();
}

BaseView* BaseView::getChild(unsigned int i) const
{
	return mChildren.at(i);
}

void BaseView::setParent(BaseView* parent)
{
	mParent = parent;
}

BaseView* BaseView::getParent() const
{
	return mParent;
}

void BaseView::animate(Animation* data)
{
	if(isAnimating()) LOG(LogLevel::Warning, "Called animate on an animating component!");

	mAnimation = data;
}

bool BaseView::isAnimating()
{
	return mAnimation != NULL && !mAnimation->completed;
}

bool BaseView::input(Input input)
{
	for(unsigned int i = 0; i < getChildCount(); i++)
	{
		if(getChild(i)->input(input))
			return true;
	}

	return false;
}

Eigen::Vector3f BaseView::getAbsolutePosition()
{
	Eigen::Vector3f result = Eigen::Vector3f::Zero();

	if(mParent)
		result = mParent->getAbsolutePosition() + mPosition;
	else
		result = mPosition;

	return result;
}

unsigned char BaseView::getAbsoluteOpacity()
{
	unsigned char result = 0;

	//the absolute opacity is computed using my parent opacity as multiplier of my opacity
	//myOpacity * myParentOpacity/255

	if(mParent)
		result = mOpacity * mParent->getAbsoluteOpacity()/255;
	else
		result = mOpacity;

	return result;
}

Eigen::Vector4i BaseView::getAbsoluteClipRect()
{
	Eigen::Vector3f abs = getAbsolutePosition();

	if(mParent != NULL)
	{
		Eigen::Vector4i pclip = mParent->getAbsoluteClipRect();

		int leftclip = max((int)pclip.x(), (int)abs.x());
		int topclip  = max((int)pclip.y(), (int)abs.y());

		int rightclip  = min((int)pclip.x() + pclip.z(), (int)(abs.x() + mSize.x()));
		int bottomclip = min((int)pclip.y() + pclip.w(), (int)(abs.y() + mSize.y()));

		int widthclip = 0;
		int heightclip = 0;

		if(rightclip > leftclip) widthclip = rightclip - leftclip;
		if(bottomclip > topclip) heightclip = bottomclip - topclip;

		Eigen::Vector4i result = Eigen::Vector4i(leftclip, topclip, widthclip, heightclip);

		return result;
	}
	else
	{
		Eigen::Vector4i result = Eigen::Vector4i((int)abs.x(),(int)abs.y(),(int)mSize.x(),(int)mSize.y());

		return result;
	}
}

void BaseView::render(const Eigen::Affine3f& parentTrans)
{
	if( ! Visible ) return;

	//trasform me
	Eigen::Affine3f trans = parentTrans * getTransform();

	//render me
	RenderController::setMatrix(trans);

	//my parent's clip rectangle is my clip rectangle, i can't draw out of that
	if(mParent != NULL)
	{
		RenderController::getInstance().pushClipRect(getAbsoluteClipRect());
	}

	//this is the draw function that should be overridden by children class
	draw();

	//remove clip rectangle if needed
	if(mParent != NULL)
		RenderController::getInstance().popClipRect();

	//render children
	for(unsigned int i = 0; i < getChildCount(); i++)
	{
		getChild(i)->render(trans);
	}
}

void BaseView::draw()
{
	//compute background colors with color's alpha and view opacity
	unsigned char absOpacity = getAbsoluteOpacity();
	int color = (mBackgroundColor>>8<<8) | (int)((mBackgroundColor & 0x000000FF) * (float)absOpacity / 255) ;
	int selectedColor = (mSelectedBackgroundColor>>8<<8) | (int)((mBackgroundColor & 0x000000FF) * (float)absOpacity / 255);

	//draw background rectangle
	//at position + parent position
	//of given size
	RenderController::drawRect(
		(int)getAbsolutePosition().x(), (int)getAbsolutePosition().y(),
		(int)mSize.x(), (int)mSize.y(),
		mSelected? selectedColor : color);
}
