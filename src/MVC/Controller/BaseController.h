/* LogController class
 *
 * Create the interface for access the Log system
 */

#ifndef _BASECONTROLLER_H_
#define _BASECONTROLLER_H_

using namespace std;

using namespace RetroEnd;

namespace RetroEnd
{
	namespace Controller
	{
		class BaseController
		{

		public:
			virtual void start() {};
			virtual void stop() {};

		protected:

		private:

		};
	}
}

#endif _BASECONTROLLER_H_