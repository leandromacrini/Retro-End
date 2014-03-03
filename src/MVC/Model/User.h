/* User Model
 *
 * Rappresent the Player and any Friend player on the Social context
 */

#pragma once

#include "../../globals.h"

namespace RetroEnd
{
	namespace Model
	{
		class User
		{
		public:
			string ID;
			string Username;
			string Password;
			string FirstName;
			string LastName;
			string EMail;

			string NetAddress;
			Uint32 NetPort;

			bool IsOnline;

			User();
		};
	}
}