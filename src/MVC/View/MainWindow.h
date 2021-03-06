#pragma once

/* Main Window class
 *
 * The Main parent of any GUI elements
 * Pass the input only to the top level child
 */

#include <queue>

#include "../../globals.h"

#include "BaseView.h"

#include "../Model/InputConfig.h"

namespace RetroEnd
{
	namespace View
	{
		class MainWindow : public BaseView
		{
		public:
			MainWindow();
			~MainWindow();

			void showConsoles();
			void showLogo();

			//check if new roms was added
			void checkForNewGames();

			//check if some roms was removed
			void checkForOldGames();

			void update(unsigned int deltaTime) override;
		private:
			BaseView* mConsoleView;

			//properties to handle async scrape
			bool mScrapeComplete;
			BaseView* mScrapeView;
			void* mScrapeResult;
		};
	}
}
