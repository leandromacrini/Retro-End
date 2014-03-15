#pragma once

#include "../../globals.h"

#include "BaseView.h"

#include "../Model/Observer.h"
#include "../Model/TextureResource.h"

namespace RetroEnd
{
	namespace View
	{
		class Switch : public BaseView
		{
		public:
			Switch(bool checked = false);

			void setChecked(bool checked);
			bool getChecked() const;
			void toggle();


			void draw() override;
			bool input(Model::Input input) override;

			//EVENTS

			//Checked valu changed event
			Model::Observer<bool> onCheckedChanged;

		private:
			bool mChecked;
			shared_ptr<Model::TextureResource> mCheckedTexture;
			shared_ptr<Model::TextureResource> mUncheckedTexture;
		};
	}

}
