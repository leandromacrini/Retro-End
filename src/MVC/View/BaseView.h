/* Base View class
*
* Create the interface for access to any GUI component
* Implements the animation system
* Implements the rendering and update systems
*/

#pragma once

#include <Eigen/Dense>

#include "../../globals.h"

#include "../Model/InputConfig.h"

using namespace std;

namespace RetroEnd
{
	namespace View
	{
		enum TextAlign
		{
			Left,
			Center,
			Right
		};

		struct Animation
		{
		public:
			Animation( ) : moveOffset(NULL), newSize(NULL), newOpacity(NULL), millisDuration(0), millisDelay(0), endCallback([](){}), completed(false) {};
			
			//Optionals
			Eigen::Vector3f* moveOffset;
			Eigen::Vector2f* newSize;
			unsigned char*   newOpacity;

			//Animation duration in milliseconds
			unsigned int millisDuration;

			//millis to wait before start the animation
			int millisDelay;

			//The function that will be called when the animation ends (to chain animations)
			function<void ()> endCallback; //this should default to nullptr but raspberry g++-4.7.x doesn't accept a nullptr as function

			//
			bool completed;
		};

		class BaseView
		{

		public:
			BaseView();
			~BaseView();

			//Called when input is received.
			//Return true if the input is consumed, false if it should continue to be passed to other children.
			virtual bool input(Model::Input input);

			//Called when time passes.  Default implementation also calls update(deltaTime) on children - so you should probably call BaseView::update(deltaTime) at some point.
			virtual void update(unsigned int deltaTime);

			//Called when it's time to render.  By default, just calls renderChildren(transform).
			//You probably want to override this like so:
			//1. Calculate the new transform that your control will draw at with Eigen::Affine3f t = parentTrans * getTransform().
			//2. Set the renderer to use that new transform as the model matrix - Renderer::setModelMatrix(t.data());
			//3. Draw your component.
			//4. Tell your children to render, based on your component's transform - renderChildren(t).
			void render(const Eigen::Affine3f& parentTrans);
			
			//this is there the GUI component is drown
			virtual void draw();

			Eigen::Vector3f getPosition() const;
			void setPosition(const Eigen::Vector3f& offset);
			void setPosition(float x, float y, float z = 0.0f);
			virtual void onPositionChanged() {};

			Eigen::Vector2f getSize() const;
			void setSize(const Eigen::Vector2f& size);
			void setSize(float w, float h);
			virtual void onSizeChanged() {};

			unsigned char getOpacity() const;
			void setOpacity(unsigned char opacity);

			unsigned int getBackgroundColor() const;
			void setBackgroundColor(unsigned int color);

			unsigned int getSelectedBackgroundColor() const;
			void setSelectedBackgroundColor(unsigned int color);

			void setParent(BaseView* parent);
			BaseView* getParent() const;

			void addChild(BaseView* cmp);
			void removeChild(BaseView* cmp);
			void removeAllChildren();
			unsigned int getChildCount() const;
			BaseView* getChild(unsigned int i) const;

			const Eigen::Affine3f getTransform();

			void animate(Animation* data);
			bool isAnimating();

			void setSelected(bool selected);
			bool getSelected() const;

			bool Visible;

			int debugID;

		protected:

			//the Parent View
			BaseView* mParent;
			
			//the view opacity
			unsigned char mOpacity;
			
			//position is relative to the parent component
			Eigen::Vector3f mPosition;
			
			//float size of the view
			Eigen::Vector2f mSize;

			//a bool that indicate if the view is actually selected
			bool mSelected;

			unsigned int mBackgroundColor;
			unsigned int mSelectedBackgroundColor;

			std::vector<BaseView*> mChildren;
		
			Eigen::Vector3f getAbsolutePosition();
			unsigned char   getAbsoluteOpacity();
			Eigen::Vector4i getAbsoluteClipRect();
		private:
			Eigen::Affine3f mTransform;
			View::Animation * mAnimation;
			
		};
	}
}
