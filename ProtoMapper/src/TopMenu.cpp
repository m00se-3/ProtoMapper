#include "TopMenu.hpp"

#include "TGUI/String.hpp"

TopMenu::TopMenu() : _group(tgui::Group::create()), _menuBar(tgui::MenuBar::create()) { SetupMenu(); }

TopMenu::TopMenu(float w, float h)
	: _group(tgui::Group::create(tgui::Layout2d{ tgui::Vector2f{ w, h } })), _menuBar(tgui::MenuBar::create())
{
	SetupMenu();
}

void TopMenu::SetupMenu()
{
	_menuBar->addMenu("File");
	_menuBar->addMenuItem("File", "New");
	_menuBar->addMenuItem("File", "Open");
	_menuBar->addMenuItem("File", "-");
	_menuBar->addMenuItem("File", "Save");
	_menuBar->addMenuItem("File", "Save As");
	_menuBar->addMenuItem("File", "-");
	_menuBar->addMenuItem("File", "Close");

	auto* ren = _menuBar->getRenderer();

	ren->setBackgroundColor(tgui::Color{ 50u, 50u, 50u });
	ren->setTextColor(tgui::Color{ 200u, 200u, 200u });



	_group->add(_menuBar);
}

tgui::Group::Ptr TopMenu::GetGroup() const { return _group; }