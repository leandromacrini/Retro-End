/* Base Model class
 *
 * Create the interface for access to any Model
 * Create the SQLite DB data layer
 */

#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include "../../globals.h"

using namespace std;

namespace RetroEnd
{
	namespace View
	{
		class MainWindow
		{
		public:
			MainWindow();
			~MainWindow();

			bool init(int width, int height);
			bool isRunning();
			void update();

		private:

		};
	}
}

#endif _MAINWINDOW_H_