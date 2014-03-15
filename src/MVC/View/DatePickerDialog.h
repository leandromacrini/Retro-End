/* MultiChoice Dialog class
*
* Create a Modal Dialog to let the user choice between the given values
* 
*/

#pragma once

#include <boost/date_time.hpp>

#include "../../globals.h"

#include "../Model/Observer.h"
#include "../Model/InputConfig.h"

#include "BaseView.h"
#include "Label.h"
#include "ListView.h"

#include "../Controller/LogController.h"

namespace RetroEnd
{
	namespace View
	{
		class DatePickerDialog : public BaseView
		{

			static vector<string>* YEARS;
			static vector<string>* MONTHS;
			static vector<string>* DAYS;
		
		public:
			DatePickerDialog(string title, string date, function<void (string date)> callback);

			//OVERRIDES

			bool input(Model::Input input) override;

		private:
			string mDate;
			function<void (string date)> mCallback;

			BaseView* mBack;

			Label* mTitle;

			ListView* mYear;
			ListView* mMonth;
			ListView* mDay;
		};
	}
}
