/* Scrape View Class
 *
 * A View that search for games informations and let the user choice witch game scrape
 */

#pragma once

#include "../../globals.h"

#include "BaseView.h"
#include "Label.h"

#include "../Controller/LogController.h"
#include "../Controller/RenderController.h"

using namespace std;

namespace RetroEnd
{
	namespace View
	{
		class MessageDialog : public BaseView
		{
		public:
			 MessageDialog	();
			~MessageDialog	();

			void setMessage(string message);

		private:
			Label* mMessage;
		};
	}
}