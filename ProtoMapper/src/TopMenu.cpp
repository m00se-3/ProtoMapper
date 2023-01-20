#include "TopMenu.hpp"

#include "TGUI/String.hpp"

TopMenu::TopMenu(ProtoMapper* par) : UIGroup(par),_menuBar(tgui::MenuBar::create()) { SetupMenu(); }

TopMenu::TopMenu(ProtoMapper* par, float w, float h)
	: UIGroup(par, w, h), _menuBar(tgui::MenuBar::create())
{
	SetupMenu();
}

void TopMenu::SetupMenu()
{
	/*
		Set up File menu.
	*/
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



	// Add MenuBar to the group
	_group->add(_menuBar);
}
