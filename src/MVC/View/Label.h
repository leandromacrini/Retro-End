#pragma once

#include "BaseView.h"

#include "../Model/Font.h"

namespace RetroEnd
{
	namespace View
	{
		class Label : public BaseView
		{
		public:
			Label();

			void setFont(std::shared_ptr<Model::Font> font);
			std::shared_ptr<Model::Font> getFont() const;
			void onSizeChanged() override;
			void setText(const std::string& text);
			void setColor(unsigned int color);
			
			TextAlign HorizontalTextAlign;
			
			bool WrapText;

			bool ShadowVisible;
			int ShadowColor;
			Eigen::Vector2i ShadowOffset;

			void draw() override;

		private:
			void calculateExtent();

			unsigned int mColor;
			std::shared_ptr<Model::Font> mFont;
			Eigen::Matrix<bool, 1, 2> mAutoCalcExtent;
			std::string mText;
		};
	}

}
