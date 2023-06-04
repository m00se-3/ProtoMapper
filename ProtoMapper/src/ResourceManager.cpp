#include "ResourceManager.hpp"

template<>
std::unordered_map<const std::string, Texture2D>& ResourceManager::GetStorageMap(const Texture2D&) { return _textures; }

template<>
std::unordered_map<const std::string, Shader>& ResourceManager::GetStorageMap(const Shader&) { return _shaders; }

template<>
std::unordered_map<ResourceManager::IDType, uint16_t>& ResourceManager::GetReferenceMap(const Texture2D&) { return _textureRefCount; }

template<>
std::unordered_map<ResourceManager::IDType, uint16_t>& ResourceManager::GetReferenceMap(const Shader&) { return _shaderRefCount; }

template<>
void ResourceManager::DestroyResource(IDType id, Texture2D temp)
{
	temp.ID = id;
	temp.Destroy();
}

template<>
void ResourceManager::DestroyResource(IDType id, Shader temp)
{
	temp.ID = id;
	temp.Destroy();
}