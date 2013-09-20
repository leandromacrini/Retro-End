#include "BaseView.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

BaseView::BaseView()
{

}

BaseView::~BaseView()
{

}

void BaseView::update(int deltaTime)
{
	for(unsigned int i = 0; i < getChildCount(); i++)
	{
		getChild(i)->update(deltaTime);
	}
}

void BaseView::render(const Eigen::Affine3f& parentTrans)
{
	//trasform me
	Eigen::Affine3f trans = parentTrans * getTransform();

	//trasform children
	for(unsigned int i = 0; i < getChildCount(); i++)
	{
		getChild(i)->render(trans);
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

const Eigen::Affine3f BaseView::getTransform()
{
	mTransform.setIdentity();
	mTransform.translate(mPosition);
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

void BaseView::animate(AnimationData data)
{

}

bool BaseView::input(InputConfigModel* config, Input input)
{
	for(unsigned int i = 0; i < getChildCount(); i++)
	{
		if(getChild(i)->input(config, input));//TODO config, input))
			return true;
	}

	return false;
}