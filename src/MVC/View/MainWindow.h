/* Main Window class
 *
 * The Main parent of any GUI elements
 * Pass the input only to the top level child
 */

#pragma once

#include "../../globals.h"

#include "../Model/InputConfig.h"

#include "BaseView.h"

namespace RetroEnd
{
	namespace View
	{
		class MainWindow : public BaseView
		{
		public:
			MainWindow();
			~MainWindow();

			bool input(Model::InputConfig* config, Model::Input input) override;

		private:
			
		};
	}
}
