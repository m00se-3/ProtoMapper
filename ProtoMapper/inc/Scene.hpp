#ifndef PROTOMAPPER_SCENE_HPP
#define PROTOMAPPER_SCENE_HPP

#include <memory>

class Map;

class Scene
{

public:
	Scene() = default;

	virtual bool Init() = 0;
	virtual bool Update() = 0;
	virtual void Cleanup();


private:

	std::shared_ptr<Map> _map;

};

#endif // !PROTOMAPPER_SCENE_HPP
