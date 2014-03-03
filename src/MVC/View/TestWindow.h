#pragma once

#include "../../globals.h"

#include "BaseView.h"
#include "../Model/InputConfig.h"

namespace RetroEnd
{
	namespace View
	{
		class TestWindow : public BaseView
		{
		public:
			TestWindow();
			~TestWindow();

			bool input(Model::Input input) override;

		private:
			BaseView* mConsoleView;
		};
	}
}