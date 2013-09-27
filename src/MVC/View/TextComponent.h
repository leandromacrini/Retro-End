#pragma once

#include "BaseView.h"
#include "../Model/Font.h"
namespace RetroEnd
{
	namespace View
	{
		class TextComponent : public BaseView
		{
		public:
			TextComponent();
			TextComponent(const std::string& text, std::shared_ptr<Model::Font> font, Eigen::Vector3f pos, Eigen::Vector2f size);

			void setFont(std::shared_ptr<Model::Font> font);
			void onSizeChanged() override;
			void setText(const std::string& text);
			void setColor(unsigned int color);
			void setCentered(bool center); //Default is uncentered.

			void draw() override;

		private:
			std::shared_ptr<Model::Font> getFont() const;

			void calculateExtent();

			unsigned int mColor;
			std::shared_ptr<Model::Font> mFont;
			Eigen::Matrix<bool, 1, 2> mAutoCalcExtent;
			std::string mText;
			bool mCentered;
		};
	}

}
