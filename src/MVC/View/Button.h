#pragma once

#include "../../globals.h"

#include "BaseView.h"
#include "Image.h"
#include "Label.h"

#include "../Model/Observer.h"
#include "../Model/TextureResource.h"

namespace RetroEnd
{
	namespace View
	{
		class Button : public BaseView
		{
		public:
			Button(string title);

			void press();

			void draw() override;
			bool input(Model::Input input) override;

			//EVENTS

			//Button pressed event
			Model::Observer<bool> onButtonPressed;

		private:
			bool mPressed;
			string mTitle;
			shared_ptr<Model::TextureResource> mTexture;
			shared_ptr<Model::TextureResource> mPressedTexture;
		};
	}

}
