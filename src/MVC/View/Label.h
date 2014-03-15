#pragma once

#include "BaseView.h"

#include "../Model/Font.h"
#include "../Model/InputConfig.h"
#include "../Model/Observer.h"

namespace RetroEnd
{
	namespace View
	{
		class Label : public BaseView
		{
		public:
			Label(bool obfuscated = false);

			void setFont(std::shared_ptr<Model::Font> font);
			std::shared_ptr<Model::Font> getFont() const;
			
			void setText(const string& text);
			string getText();
			void setColor(unsigned int color);
			
			TextAlign HorizontalTextAlign;
			
			bool WrapText;

			bool ShadowVisible;
			int ShadowColor;
			Eigen::Vector2i ShadowOffset;

			//OVERRIDES

			void draw() override;
			bool input(Model::Input input) override;

			//EVENTS

			//Button pressed on label
			Model::Observer<int> onLabelPressed;
		protected:
			void onSizeChanged() override;

		private:
			bool mObfuscated;
			void calculateExtent();

			unsigned int mColor;
			std::shared_ptr<Model::Font> mFont;
			Eigen::Matrix<bool, 1, 2> mAutoCalcExtent;
			std::string mText;
		};
	}

}
