#ifndef PROTOMAPPER_RENDERER_HPP
#define PROTOMAPPER_RENDERER_HPP

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <string>
#include <functional>

#include <memory>

// Forward declarations.
struct Vertex2D;
class Buffer2D;
class Shader;
class Texture2D;

class Renderer
{
public:
	enum class mode;

private:
	glm::mat4 _model, _view, _projection;
	mode _currentMode = mode::Two;
	float _vWidth = 0.f, _vHeight = 0.f;

	Buffer2D* _buffer = nullptr;
	Shader* _shader = nullptr;
	Texture2D* _texture = nullptr;

	std::unique_ptr<Shader> _defaultShader;
	std::unique_ptr<Texture2D> _defaultTexture;

	std::function<void()> _uniforms = []() {};

public:

	enum class mode
	{
		Two,
		Three
	};

	Renderer(const std::string& dir);

	mode GetRenderMode() const;

	bool Init(mode newMode);
	void Begin();
	void End();
	void SetRenderWindow(float w, float h);
	void SetUniforms(const std::function<void()>& uniforms);
	void SetViewport(int x, int y, int w, int h);
	void SetRenderMode(mode m);
	void UseTexture(Texture2D* texture = nullptr);
	void UseShader(Shader* shader = nullptr);
	void UseBuffer(Buffer2D* buffer = nullptr);

	void DrawCurrentBuffer();
};

#endif // !PROTOMAPPER_RENDERER_HPP
