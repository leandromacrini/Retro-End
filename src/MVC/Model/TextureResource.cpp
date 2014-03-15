#include "TextureResource.h"

#include "../Controller/LogController.h"
#include "../Controller/RenderController.h"
#include "../Controller/ResourceController.h"

#include GLHEADER

#include "ImageIO.h"

using namespace RetroEnd::Model;
using namespace RetroEnd::Controller;

map< string, weak_ptr<TextureResource> > TextureResource::sTextureMap;

TextureResource::TextureResource(const string& path) : mTextureID(0), mPath(path), mTextureSize(Eigen::Vector2i::Zero())
{
	reload();
}

TextureResource::~TextureResource()
{
	deinit();
}

void TextureResource::unload()
{
	deinit();
}

void TextureResource::reload()
{
	if(!mPath.empty())
		initFromResource(ResourceController::getInstance().getFileData(mPath));
}

void TextureResource::initFromResource(const ResourceData data)
{
	//make sure we aren't going to leak an old texture
	deinit();

	size_t width, height;
	vector<unsigned char> imageRGBA = ImageIO::loadFromMemoryRGBA32(const_cast<unsigned char*>(data.ptr.get()), data.length, width, height);

	if(imageRGBA.size() == 0)
	{
		LOG(LogLevel::Error, "Could not initialize texture (invalid resource data)!");
		return;
	}

	//now for the openGL texture stuff
	glGenTextures(1, &mTextureID);

	if(mTextureID == 0) //error?
		LOG(LogLevel::Error , "Error in texture generation. Error code: " + to_string(glGetError()));

	glBindTexture(GL_TEXTURE_2D, mTextureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageRGBA.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	mTextureSize << width, height;
}

GLuint TextureResource::getTextureID()
{
	return mTextureID;
}

void TextureResource::initFromScreen()
{
	deinit();

	int width = RenderController::getInstance().getScreenWidth();
	int height = RenderController::getInstance().getScreenHeight();

	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D, mTextureID);

	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, width, height, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	mTextureSize[0] = height;
	mTextureSize[1] = height;
}

void TextureResource::deinit()
{
	if(mTextureID != 0)
	{
		glDeleteTextures(1, &mTextureID);
		mTextureID = 0;
	}
}

Eigen::Vector2i TextureResource::getSize() const
{
	return mTextureSize;
}

shared_ptr<TextureResource> TextureResource::get(const string& path)
{
	if(path.empty())
	{
		shared_ptr<TextureResource> tex(new TextureResource(""));
		ResourceController::getInstance().addReloadable(tex); //make sure we're deinitialized even though we do nothing on reinitialization
		return tex;
	}

	auto foundTexture = sTextureMap.find(path);
	if(foundTexture != sTextureMap.end())
	{
		if(!foundTexture->second.expired())
		{
			return foundTexture->second.lock();
		}
	}

	shared_ptr<TextureResource> tex = shared_ptr<TextureResource>(new TextureResource(path));
	sTextureMap[path] = weak_ptr<TextureResource>(tex);
	ResourceController::getInstance().addReloadable(tex);
	return tex;
}
