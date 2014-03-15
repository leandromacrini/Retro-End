/* Image View class
 *
 * A simple view that can load and show and image
 */

#pragma once

#include "../../globals.h"

#include <memory>

#include "BaseView.h"
#include "../Model/TextureResource.h"

using namespace std;

namespace RetroEnd
{
	namespace View
	{
		class Image : public BaseView
		{
		public:
			 Image	();
			~Image	();

			void	setPath (string image);
			string& getPath ();

			void	setTiled (bool tiled);
			bool	getTiled ();

			void resizeByWidth(float width);
			void resizeByHeight(float height);

			//You can get the rendered size of the ImageComponent with getSize().
			Eigen::Vector2i getTextureSize() const;

			void	draw () override;

		private:
			string mPath;
			bool mTiled;
			shared_ptr<Model::TextureResource> mTexture;
		};
	}
}