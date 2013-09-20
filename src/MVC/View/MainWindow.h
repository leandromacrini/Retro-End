/* Main Window class
 *
 * The Main parent of any GUI elements
 * Pass the input only to the top level child
 */

#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include "../../globals.h"

#include "BaseView.h"
#include "../Model/InputConfigModel.h"

using namespace std;

namespace RetroEnd
{
	namespace View
	{
		class MainWindow : public BaseView
		{
		public:
			MainWindow();
			~MainWindow();

			bool input(Model::InputConfigModel* config, Model::Input input);

			bool isRunning();

		private:

		};
	}
}

#endif _MAINWINDOW_H_