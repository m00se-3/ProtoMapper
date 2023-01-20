#include "UIGroup.hpp"
#include "ProtoMapper.hpp"

UIGroup::UIGroup(ProtoMapper* par) : _group(tgui::Group::create()), _parent(par) {}

UIGroup::UIGroup(ProtoMapper* par, float w, float h)
	: _group(tgui::Group::create(tgui::Layout2d{ tgui::Vector2f{ w, h } })), _parent(par)
{

}

UIGroup::~UIGroup() 
{
	
}

tgui::Group::Ptr UIGroup::GetGroup() const { return _group; }

ProtoMapper* UIGroup::GetParent() const { return _parent; }