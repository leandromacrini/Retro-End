#pragma once

#include "../../globals.h"

#include <Eigen/Dense>
#include GLHEADER

#include "../Controller/ResourceController.h"

namespace RetroEnd
{
	namespace Model
	{
		class TextureResource : public Controller::IReloadable
		{
		public:
			static shared_ptr<TextureResource> get(const string& path);

			virtual ~TextureResource();

			void unload() override;
			void reload() override;

			Eigen::Vector2i getSize() const;

			void initFromScreen();
			GLuint getTextureID();

		private:
			TextureResource(const string& path);

			void initFromPath();
			void initFromResource(const Controller::ResourceData data);
			void deinit();

			Eigen::Vector2i mTextureSize;
			GLuint mTextureID;
			const string mPath;

			static map< string, weak_ptr<TextureResource> > sTextureMap;
		};

	}
}
