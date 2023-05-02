#ifndef MAPPER_UIGROUP_HPP
#define MAPPER_UIGROUP_HPP

#include "TGUI/Core.hpp"
#include "TGUI/Backends/SDL.hpp"
#include "TGUI/Widgets/Group.hpp"

class ProtoMapper;

class UIGroup
{
protected:
	tgui::Group::Ptr _group;
	ProtoMapper* _parent = nullptr;

public:
	UIGroup(ProtoMapper*);
	UIGroup(ProtoMapper* , float, float);
	virtual ~UIGroup();

	tgui::Group::Ptr GetGroup() const;
	ProtoMapper* GetParent() const;
};

#endif