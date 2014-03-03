/* LogController class
 *
 * Create the interface for access the Log system
 */

#pragma once

using namespace std;

using namespace RetroEnd;

namespace RetroEnd
{
	namespace Controller
	{
		class BaseController
		{

		public:
			BaseController() : mActivated(false) { };

			virtual void start() {};
			virtual void stop() {};

			virtual void activate()
			{
				mActivated = true;
			}

			virtual void deactivate()
			{
				mActivated = true;
			}

		protected:
			bool mActivated;
		private:
			
		};
	}
}
