#include "MainWindow.h"

using namespace RetroEnd::View;

MainWindow::MainWindow()
{
	
}

MainWindow::~MainWindow()
{

}

bool MainWindow::init(int width, int height)
{
	//if(!Renderer::init(width, height))
	//{
	//	LOG(LogLevel::Fatal, "Renderer failed to initialize!";
	//	return false;
	//}

	//mInputManager->init();

	//mResourceManager.reloadAll();

	////keep a reference to the default fonts, so they don't keep getting destroyed/recreated
	//if(mDefaultFonts.empty())
	//{
	//	mDefaultFonts.push_back(Font::get(mResourceManager, Font::getDefaultPath(), FONT_SIZE_SMALL));
	//	mDefaultFonts.push_back(Font::get(mResourceManager, Font::getDefaultPath(), FONT_SIZE_MEDIUM));
	//	mDefaultFonts.push_back(Font::get(mResourceManager, Font::getDefaultPath(), FONT_SIZE_LARGE));
	//}

	return true;
}

bool MainWindow::isRunning()
{

	return true;
}

void MainWindow::update()
{

}