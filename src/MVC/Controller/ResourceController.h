/* Resource Controller class
*
* Allow loading resources embedded into the executable like an actual file.
* Allow embedded resources to be optionally remapped to actual files for further customization.
* Allow loading of reusable resources to avoid memory usage
*/

#pragma once

#include "../../globals.h"

#include <stddef.h>
#include <memory>
#include <map>
#include <list>

#include "BaseController.h"
#include "GamingController.h"

using namespace std;
using namespace RetroEnd;

namespace RetroEnd
{
	namespace Controller
	{
		struct ResourceData
		{
			const std::shared_ptr<unsigned char> ptr;
			const size_t length;
		};

		class IReloadable
		{
		public:
			virtual void unload() = 0;
			virtual void reload() = 0;
		};

		class ResourceController : public BaseController
		{
		public:
			static ResourceController& getInstance()
			{
				static ResourceController instance;
				return instance;
			}

			void addReloadable(std::weak_ptr<IReloadable> reloadable);

			void unloadAll();
			void reloadAll();

			void start();

			const ResourceData getFileData(const std::string& path) const;
			static bool fileExists(const std::string& path);

		private:
			ResourceData loadFile(const std::string& path) const;

			std::list< std::weak_ptr<IReloadable> > mReloadables;


			//---Singleton---
			ResourceController mInstance();

			ResourceController() {}; //private instance costructor for Singleton Controller
			ResourceController(ResourceController const&);// Don't Implement
			void operator=(ResourceController const&); // Don't implement
			//---Singleton---
		};
	}
}
