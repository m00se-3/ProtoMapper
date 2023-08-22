#include "UIContainer.hpp"

#include "Vertex.hpp"
#include "ResourceManager.hpp"
#include "Renderer.hpp"

Texture2DManager* UIContainer::_texMan = nullptr;


void UIContainer::SetResourceManager(Texture2DManager* ptr) { _texMan = ptr; }

UIContainer::UIContainer()
{
	std::string fontFile = ROOT_DIR;
	fontFile += "/assets/fonts/keep_calm/KeepCalm-Medium.ttf";

	int imgWidth, imgHeight;
	_fontTexture = _texMan->Load("KeepCalm-Medium");
	_fontTexture.Create();
	auto nTexture = _texMan->Get("default");


}

UIContainer::~UIContainer()
{
	_fontTexture.Reset();
	_texMan->Unload("KeepCalm-Medium");

	if (_updateUIData)
	{
		_uiData.SaveFile(_dataFile.c_str());
	}

	_uiData.Reset();
}

bool UIContainer::SetData(const std::filesystem::path& filepath)
{
	if (std::filesystem::exists(filepath) && filepath.extension() == ".ini")
	{
		_dataFile = filepath.string();
		_uiData.LoadFile(_dataFile.c_str());

		return true;
	}

	return false;
}

void UIContainer::Lock() { _mutex.lock(); }

void UIContainer::Unlock() { _mutex.unlock(); }

void UIContainer::CompileUI()
{
	
}

void UIContainer::DrawUI(Renderer* ren)
{
	

}

void UIContainer::UpdateUI(float wWidth, float wHeight)
{

	

}
