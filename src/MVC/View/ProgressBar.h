#pragma once

#include "../../globals.h"

#include "BaseView.h"

#include "../Model/Observer.h"
#include "../Model/TextureResource.h"

namespace RetroEnd
{
	namespace View
	{
		class ProgressBar : public BaseView
		{
		public:
			ProgressBar();

			//set the value (0 -> 100)
			void setValue(int value);
			int getValue() const;

			void draw() override;
			bool input(Model::Input input) override;

			//EVENTS

			//Value changed event
			Model::Observer<int> onValueChanged;

		private:
			int mValue;
			shared_ptr<Model::TextureResource> mBorderTexture;
			shared_ptr<Model::TextureResource> mValueTexture;
		};
	}

}
