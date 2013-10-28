#include "Sprite.h"

#include "../Controller/RenderController.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

Sprite::Sprite() : mCurrentFrame(0), mFrameDuration(100), mTimeElapsedFromLastFrame(0), mFrameCount(0), mActive(false), mLoop(true)
{

	FrameHeight = 0;
}

string& Sprite::getPath()
{
	return mPath;
}

void Sprite::start()
{
	mActive = true;
	mCurrentFrame = 0;
}

void Sprite::stop()
{
	mActive = false;
}

void Sprite::setLoop(bool loop)
{
	mLoop = loop;
}

bool Sprite::getLoop()
{
	return mLoop;
}

void Sprite::setFrameDuration(int rate)
{
	mFrameDuration = rate;
}

int Sprite::getFrameDuration()
{
	return mFrameDuration;
}

void Sprite::setPath(string image)
{
	if(mPath != image)
	{
		mPath = image;

		if(mPath.empty() || ! ResourceController::fileExists(mPath))
			mTexture.reset();
		else
		{
			mTexture = TextureResource::get(mPath);

			//compute the frame count and size
			mFrameCount = mTexture->getSize().y() / (FrameHeight==0? mTexture->getSize().x() : FrameHeight);
		}
	}

}

void Sprite::update(unsigned int deltaTime)
{
	BaseView::update(deltaTime);

	if(mActive)
	{
		mTimeElapsedFromLastFrame+= deltaTime;

		//check if we have to change frame
		if(mTimeElapsedFromLastFrame >= mFrameDuration)
		{
			mCurrentFrame++;
			mTimeElapsedFromLastFrame = 0;

			//check if sprite is ended
			if(mCurrentFrame >= mFrameCount)
			{
				if(mLoop)
				{
					mCurrentFrame = 0;
				}
				else
				{
					mCurrentFrame = mFrameCount-1;
					stop();
				}

			}
		}

	}
}

void Sprite::draw()
{
	//draw the parent
	BaseView::draw();

	//the draw me
	if(mTexture && getOpacity() > 0)
	{
		GLfloat points[12], texs[12];
		GLubyte colors[6*4];

		RenderController::buildGLColorArray(colors, 0xFFFFFF00 | mOpacity, 6);
		buildImageArray(getAbsolutePosition().x(), getAbsolutePosition().y(), points, texs);

		drawImageArray(points, texs, colors, 6);
	}
}

void Sprite::buildImageArray(float posX, float posY, GLfloat* points, GLfloat* texs)
{
	points[0]  = posX;				points[1]  = posY;
	points[2]  = posX;				points[3]  = posY + mSize.y();
	points[4]  = posX + mSize.x();	points[5]  = posY;

	points[6]  = posX + mSize.x();	points[7]  = posY;
	points[8]  = posX;				points[9]  = posY + mSize.y();
	points[10] = posX + mSize.x();	points[11] = posY + mSize.y();

	float yEnd = 1-(float)mCurrentFrame / (float)mFrameCount;
	float yStart = 1-(float)(mCurrentFrame+1) / (float)mFrameCount;

	texs[0] = 0;		texs[1] = yEnd;
	texs[2] = 0;		texs[3] = yStart;
	texs[4] = 1;		texs[5] = yEnd;

	texs[6] = 1;		texs[7] = yEnd;
	texs[8] = 0;		texs[9] = yStart;
	texs[10] = 1;		texs[11] = yStart;
}

void Sprite::drawImageArray(GLfloat* points, GLfloat* texs, GLubyte* colors, unsigned int numArrays)
{
	mTexture->bind();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	if(colors != NULL)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors);
	}

	glVertexPointer(2, GL_FLOAT, 0, points);
	glTexCoordPointer(2, GL_FLOAT, 0, texs);

	glDrawArrays(GL_TRIANGLES, 0, numArrays);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	if(colors != NULL)
		glDisableClientState(GL_COLOR_ARRAY);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}
