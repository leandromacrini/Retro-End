#pragma once

#include "../../globals.h"

#include "../Model/Sound.h"

#include "BaseView.h"

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
		};

	}

}