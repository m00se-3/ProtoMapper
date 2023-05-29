#include "Scene.hpp"

Rectangle::Rectangle(float x, float y, float w, float h)
	: x(x), y(y), w(w), h(h)
{
}

bool Rectangle::operator==(const Rectangle& other) const
{
	return (x == other.x && y == other.y && w == other.w && h == other.h);
}

bool Rectangle::Overlaps(const Rectangle& other) const
{
	return (
		x < other.x + other.w && y < other.y + other.h
		&& x + w > other.x && y + h > other.y
		);
}

bool Rectangle::Contains(const Rectangle& other) const
{
	return (
		x < other.x && x + w > other.x + other.w
		&& y < other.y && y + h > other.y + other.h
		);
}



bool Scene::Init()
{
	/*
		Create a SceneNode that represents the default background if there are no open files.
	*/


	
	return true;
}

void Scene::Update(float dt)
{
}

void Scene::Draw()
{
}

void Scene::Cleanup()
{
	root->Destroy();
	delete root;
}

SceneNode::SceneNode(SceneNode* par)
	: parent(par)
{

}

void SceneNode::Update(float dt)
{
}

void SceneNode::Draw()
{
}

void SceneNode::Destroy()
{
	for (auto* child : children)
	{
		child->Destroy();
		delete child;
	}
}
