#include "BaseView.h"

#include "../Controller/RenderController.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

BaseView::BaseView() : mParent(NULL), mOpacity(255), mPosition(Eigen::Vector3f::Zero()), mSize(Eigen::Vector2f::Zero()), mAnimation(NULL), mBackgroundColor(0x00000000)
{

}

BaseView::~BaseView()
{

}

void BaseView::update(unsigned int deltaTime)
{
	//animation handling
	if(mAnimation != NULL)
	{
		//Moving
		if(mAnimation->moveOffset != NULL)
		{
			float deltaX = mAnimation->moveOffset->x() * deltaTime / ( mAnimation->millisDuration > 0? mAnimation->millisDuration : 1 );
			float deltaY = mAnimation->moveOffset->y() * deltaTime / ( mAnimation->millisDuration > 0? mAnimation->millisDuration : 1 );

			//last time?
			if( abs(deltaX) > abs(mAnimation->moveOffset->x())) deltaX = mAnimation->moveOffset->x();
			if( abs(deltaY) > abs(mAnimation->moveOffset->y())) deltaX = mAnimation->moveOffset->y();

			*(mAnimation->moveOffset) -= Eigen::Vector3f(deltaX, deltaY, 0);
			mPosition += Eigen::Vector3f(deltaX, deltaY, 0);
		}

		mAnimation->millisDuration -= deltaTime >= mAnimation->millisDuration ? mAnimation->millisDuration : deltaTime ;

		//is animation completed?
		if(mAnimation->millisDuration == 0)
		{
			//save callback
			auto callback = mAnimation->endCallback;

			//delete current animation so we can chain another animation in the callback
			delete mAnimation;
			mAnimation = NULL;

			//call the end callback
			callback();
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

void BaseView::removeChild(BaseView* cmp)
{
	for(auto i = mChildren.begin(); i != mChildren.end(); i++)
	{
		if(*i == cmp)
		{
			cmp->setParent(NULL);
			mChildren.erase(i);
			return;
		}
	}
}

void BaseView::clearChildren()
{
	mChildren.clear();
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
	if(mAnimation != NULL) LOG(LogLevel::Warning, "Called animate on an animating component!");

	mAnimation = data;
}

bool BaseView::input(InputConfig* config, Input input)
{
	for(unsigned int i = 0; i < getChildCount(); i++)
	{
		if(getChild(i)->input(config, input))//TODO config, input))
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

void BaseView::render(const Eigen::Affine3f& parentTrans)
{
	//trasform me
	Eigen::Affine3f trans = parentTrans * getTransform();

	//render me
	RenderController::setMatrix(trans);
	draw();

	//trasform children
	for(unsigned int i = 0; i < getChildCount(); i++)
	{
		getChild(i)->render(trans);
	}
}

void BaseView::draw()
{
	RenderController::drawRect((int)getAbsolutePosition().x(), (int)getAbsolutePosition().y(), (int)mSize.x(), (int)mSize.y(), mBackgroundColor);
}