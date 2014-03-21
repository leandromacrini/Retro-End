#pragma once

#include <deque>
#include <mutex>
#include <boost/function.hpp>
#include <boost/function_equal.hpp>

using namespace std;

namespace RetroEnd
{
	namespace Model
	{
		template <typename T> class Observer
		{
			std::deque<boost::function<void (T arg)>> observers;

		public:
			Observer() : observers() {};

			Observer& operator +=(function<void (T arg)> f)
			{
				lock_guard<mutex> lock(mMutex);
				observers.push_back(f);
				return *this;
			}

			Observer& operator -=(function<void (T arg)> f)
			{
				lock_guard<mutex> lock(mMutex);
				for(auto it = observers.begin(); it != observers.end(); it++)
				{
					if(boost::function_equal(f, *it))
					{
						observers.erase(it);
						break;
					}
				}
				return *this;
			}

			void operator()(T arg) 
			{
				lock_guard<mutex> lock(mMutex);
				for (auto it(observers.begin()); it != observers.end(); ++it)
					(*it)(arg);

			}
		private:
			mutex mMutex;
		};
	}
}
