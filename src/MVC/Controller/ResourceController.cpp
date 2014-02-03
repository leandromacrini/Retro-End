#include "ResourceController.h"

#include "LogController.h"
#include <fstream>
#include <boost/filesystem.hpp>

using namespace RetroEnd::Model;
using namespace RetroEnd::View;
using namespace RetroEnd::Controller;

namespace fs = boost::filesystem;

auto array_deleter = [](unsigned char* p) { delete[] p; };
auto nop_deleter = [](unsigned char* p) { };

const ResourceData ResourceController::getFileData(const std::string& path) const
{
	if(!fs::exists(path))
	{
		//if the file doesn't exist, return an "empty" ResourceData
		ResourceData data = {NULL, 0};
		return data;
	}else{
		ResourceData data = loadFile(path);
		return data;
	}
}

ResourceData ResourceController::loadFile(const std::string& path) const
{
	std::ifstream stream(path, std::ios::binary);

	stream.seekg(0, stream.end);
	size_t size = (size_t)stream.tellg();
	stream.seekg(0, stream.beg);

	//supply custom deleter to properly free array
	std::shared_ptr<unsigned char> data(new unsigned char[size], array_deleter);
	stream.read((char*)data.get(), size);
	stream.close();

	ResourceData ret = {data, size};
	return ret;
}

bool ResourceController::fileExists(const std::string& path)
{
	return fs::exists(path);
}

void ResourceController::start()
{
	GamingController::getInstance().onGameEnd += [this](Game& game)
	{
		unloadAll();
		reloadAll();
	};
}


void ResourceController::unloadAll()
{
	auto iter = mReloadables.begin();
	while(iter != mReloadables.end())
	{
		if(!iter->expired())
		{
			iter->lock()->unload();
			iter++;
		}else{
			mReloadables.erase(iter++);
		}
	}
}

void ResourceController::reloadAll()
{
	auto iter = mReloadables.begin();
	while(iter != mReloadables.end())
	{
		if(!iter->expired())
		{
			iter->lock()->reload();
			iter++;
		}else{
			mReloadables.erase(iter++);
		}
	}
}

void ResourceController::addReloadable(std::weak_ptr<IReloadable> reloadable)
{
	mReloadables.push_back(reloadable);
}
