#pragma once

#include <deque>

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
				observers.push_back(f);
				return *this;
			}

			void operator()(T arg) 
			{ 
				for (auto it(observers.begin()); it != observers.end(); ++it)
					(*it)(arg);
			}
		};
	}
}
