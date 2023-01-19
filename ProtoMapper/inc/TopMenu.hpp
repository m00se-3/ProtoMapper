#ifndef MAPPER_MENUBAR_HPP
#define MAPPER_MENUBAR_HPP

#include "TGUI/Core.hpp"
#include "TGUI/Backends/SFML.hpp"
#include "TGUI/Widgets/Group.hpp"
#include "TGUI/Widgets/MenuBar.hpp"

class TopMenu
{
	tgui::Group::Ptr _group;
	tgui::MenuBar::Ptr _menuBar;

public:
	TopMenu();
	TopMenu(float, float);
	~TopMenu() = default;

	void SetupMenu();
	tgui::Group::Ptr GetGroup() const;
};

#endif // !MAPPER_MENUBAR_HPP
