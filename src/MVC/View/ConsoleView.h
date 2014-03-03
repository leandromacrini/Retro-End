#pragma once

#include "../../globals.h"

#include "../Model/Observer.h"
#include "../Model/Sound.h"
#include "../Model/Device.h"

#include "BaseView.h"
#include "Sprite.h"
#include "Image.h"
#include "Label.h"

namespace RetroEnd
{
	namespace View
	{
		class ConsoleView : public BaseView
		{
		public:
			ConsoleView();
			~ConsoleView();

			void move(int direction); // 1 Next | -1 Prev

			//EVENTS

			Model::Observer<Model::Device> onOpenConsoleInfo;	//open Console info event
			Model::Observer<int> onOpenConsoleHelpScreen;		//open Console help screen
			Model::Observer<Model::Device> onOpenGamesList;		//open Game List 

			bool input(Model::Input input) override;
		private:
			void updateCurrentConsoleData();

			bool mAnimating;
			unsigned int mCurrentIndex;
			vector<Model::Device> mDevices;
			BaseView* mContainer;
			
			shared_ptr<Model::Sound> mMoveSound;
			shared_ptr<Model::Sound> mEndSound;
			shared_ptr<Model::Sound> mSelectSound;

			BaseView* mInfoContainer;

			Image* mLogo;
			Image* mMediaIcon1;
			Image* mMediaIcon2;
			Image* mControllerIcon1;
			Image* mControllerIcon2;
			
			Label* mDateManufacturer;
		};

	}

}