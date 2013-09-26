#include "Image.h"

#include "../Controller/RenderController.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

Image::Image() : mTiled(false)
{

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
			mTexture = TextureResource::get(mPath);
	}

}

Eigen::Vector2i Image::getTextureSize() const
{
	if(mTexture)
		return mTexture->getSize();
	else
		return Eigen::Vector2i(0, 0);
}

void Image::draw()
{
	//draw the parent
	BaseView::draw();

	//the draw me
	
	if(mTexture && getOpacity() > 0)
	{
		GLfloat points[12], texs[12];
		GLubyte colors[6*4];

		if(mTiled)
		{
			float xCount = mSize.x() / getTextureSize().x();
			float yCount = mSize.y() / getTextureSize().y();
			
			RenderController::buildGLColorArray(colors, (mColorShift >> 8 << 8) | (getOpacity()), 6);
			buildImageArray(0, 0, points, texs, xCount, yCount);
		}else{
			RenderController::buildGLColorArray(colors, (mColorShift >> 8 << 8) | (getOpacity()), 6);
			buildImageArray(getAbsolutePosition().x(), getAbsolutePosition().y(), points, texs);
		}

		drawImageArray(points, texs, colors, 6);
	}
}

void Image::buildImageArray(float posX, float posY, GLfloat* points, GLfloat* texs, float px, float py)
{
	points[0]  = posX;				points[1]  = posY;
	points[2]  = posX;				points[3]  = posY + mSize.y();
	points[4]  = posX + mSize.x();	points[5]  = posY;

	points[6]  = posX + mSize.x();	points[7]  = posY;
	points[8]  = posX;				points[9]  = posY + mSize.y();
	points[10] = posX + mSize.x();	points[11] = posY + mSize.y();



	texs[0] = 0;		texs[1] = py;
	texs[2] = 0;		texs[3] = 0;
	texs[4] = px;		texs[5] = py;

	texs[6] = px;		texs[7] = py;
	texs[8] = 0;		texs[9] = 0;
	texs[10] = px;		texs[11] = 0;
}

void Image::drawImageArray(GLfloat* points, GLfloat* texs, GLubyte* colors, unsigned int numArrays)
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
