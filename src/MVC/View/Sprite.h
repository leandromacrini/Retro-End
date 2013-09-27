/* Sprite View class
 *
 * A Sprite view that can load and show and images and animate between sprites
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
		class Sprite : public BaseView
		{
		public:
			 Sprite	();
			~Sprite	();

			void	setPath (string image);
			string& getPath ();

			void	setFrameDuration	(int rate);
			int		getFrameDuration	();

			void	setCurrentFrame	(int frame);
			int		getCurrentFrame	();
		
			void	setLoop	(bool loop);
			bool	getLoop	();

			void	start	();
			void	stop	();
			
			void update(unsigned int deltaTime) override;

		protected:
			void	draw () override;

		private:
			string mPath;
			shared_ptr<Model::TextureResource> mTexture;

			bool mActive, mLoop;

			int mCurrentFrame, mFrameDuration, mTimeElapsedFromLastFrame, mFrameCount;

			void buildImageArray(float x, float y, GLfloat* points, GLfloat* texs); //writes 12 GLfloat points and 12 GLfloat texture coordinates to a given array at a given position
			void drawImageArray(GLfloat* points, GLfloat* texs, GLubyte* colors, unsigned int count = 6); //draws the given set of points and texture coordinates, number of coordinate pairs may be specified (default 6)
		};
	}
}