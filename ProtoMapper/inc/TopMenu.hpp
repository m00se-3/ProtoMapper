#ifndef MAPPER_MENUBAR_HPP
#define MAPPER_MENUBAR_HPP

#include "UIGroup.hpp"
#include "TGUI/Core.hpp"
#include "TGUI/Backends/SDL.hpp"
#include "TGUI/Widgets/MenuBar.hpp"

class TopMenu : public UIGroup
{
	tgui::MenuBar::Ptr _menuBar;

public:
	TopMenu(ProtoMapper*);
	TopMenu(ProtoMapper* , float, float);
	~TopMenu() = default;

	void SetupMenu();
};

#endif // !MAPPER_MENUBAR_HPP
