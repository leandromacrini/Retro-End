#pragma once

#include <deque>
#include <boost/filesystem.hpp>
#include <mutex>

using namespace std;

namespace RetroEnd
{
	namespace Model
	{
		template <typename T> class Observer
		{
			std::deque<function<void (T arg)>> observers;

		public:
			Observer& operator +=(function<void (T arg)> f)
			{
				mMutex.lock();
				observers.push_back(f);
				mMutex.unlock();
				return *this;
			}

			Observer& operator -=(function<void (T arg)> f)
			{
				mMutex.lock();
				observers.erase(find(observers.begin(), observers.end(), f));
				mMutex.unlock();
				return *this;
			}

			void operator()(T arg) 
			{
				mMutex.lock();
				for (auto it(observers.begin()); it != observers.end(); ++it)
					(*it)(arg);
				mMutex.unlock();
			}
		private:
			std::mutex mMutex;
		};
	}
}
