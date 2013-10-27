#pragma once

#include "../../globals.h"

#include "../Model/Sound.h"

#include "BaseView.h"
#include "Sprite.h"
#include "Image.h"
#include "Label.h"

namespace RetroEnd
{
	namespace View
	{
		class ConsoleView : public BaseView
		{
		public:
			ConsoleView();
			~ConsoleView();

			bool input(Model::InputConfig* config, Model::Input input) override;
		private:
			bool mAnimating;
			BaseView* mContainer;
			
			shared_ptr<Model::Sound> mMoveSound;
			shared_ptr<Model::Sound> mSelectSound;

			BaseView* mInfoContainer;

			Image* mLogo;
			Image* mMediaIcon;
			Image* mControllerIcon;
			Image* mPlayersIcon;
			
			Label* mName;
			Label* mDateCustomer;

			Sprite* mLeftLegend;
			Sprite* mRightLegend;
		};

	}

}