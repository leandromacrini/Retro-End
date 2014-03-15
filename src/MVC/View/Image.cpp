#include "Image.h"

#include "../Controller/RenderController.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

Image::Image() : mTiled(false)
{

	mSize = Eigen::Vector2f::Zero();
}

string& Image::getPath()
{
	return mPath;
}

void Image::setPath(string image)
{
	if(mPath != image)
	{
		mPath = image;

		if(mPath.empty() || ! ResourceController::fileExists(mPath))
			mTexture.reset();
		else
		{
			LOG(LogLevel::Debug, "Image::setPath - Creating TextureResource from: " + mPath);
			mTexture = TextureResource::get(mPath);

			//update size if was Zero
			if(mSize == Eigen::Vector2f::Zero()){
				LOG(LogLevel::Debug, "Image::setPath - Size is not set, use Texture size.");
				setSize( (float)mTexture->getSize().x(), (float)mTexture->getSize().y());
			}
		}
	}

}

void Image::setTiled (bool tiled)
{
	mTiled = tiled;
}
bool Image::getTiled ()
{
	return mTiled;
}

Eigen::Vector2i Image::getTextureSize() const
{
	if(mTexture)
		return mTexture->getSize();
	else
		return Eigen::Vector2i::Zero();
}

void Image::resizeByWidth(float width)
{
	Eigen::Vector2i tSize = getTextureSize();

	if( tSize == Eigen::Vector2i::Zero()) throw "Invalid Operation, a texture is needed!";

	float height = width * tSize.y() / tSize.x();

	setSize(width, height);
}

void Image::resizeByHeight(float height)
{
	Eigen::Vector2i tSize = getTextureSize();

	if( tSize == Eigen::Vector2i::Zero()) throw "Invalid Operation, a texture is needed!";

	float width = height * tSize.x() / tSize.y();

	setSize(width, height);
}

void Image::draw()
{
	//draw the parent
	BaseView::draw();

	//the draw me
	//with opacity *= parent opacity (recursively)
	if(mTexture && getOpacity() > 0)
	{
		GLfloat points[12], texs[12];
		GLubyte colors[6*4];

		if(mTiled)
		{
			float xCount = mSize.x() / getTextureSize().x();
			float yCount = mSize.y() / getTextureSize().y();
			
			RenderController::buildGLColorArray(colors, 0xFFFFFF00 | getAbsoluteOpacity(), 6);
			RenderController::buildImageArray(getAbsolutePosition().x(), getAbsolutePosition().y(), points, texs, mSize, xCount, yCount);
		}else{
			RenderController::buildGLColorArray(colors, 0xFFFFFF00 | getAbsoluteOpacity(), 6);
			RenderController::buildImageArray(getAbsolutePosition().x(), getAbsolutePosition().y(), points, texs, mSize);
		}

		RenderController::drawImageArray(mTexture->getTextureID(), points, texs, colors, 6);
	}
}
