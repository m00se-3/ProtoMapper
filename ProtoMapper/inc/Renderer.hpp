#ifndef PROTOMAPPER_RENDERER_HPP
#define PROTOMAPPER_RENDERER_HPP

#include "Vertex.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <string>
#include <functional>
#include <type_traits>


class Renderer
{
public:
	enum class mode;

private:
	glm::mat4 _model, _view, _projection;
	mode _currentMode = mode::Two;
	float _vWidth = 0.f, _vHeight = 0.f;

	Shader* _shader = nullptr;
	Texture2D* _texture = nullptr;

	Shader _defaultShader;
	Texture2D _defaultTexture;

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
	
	template<typename VertexType>
	void DrawBuffer(const Buffer<VertexType>& buffer, unsigned int drawMode = GL_TRIANGLES)
	{		
		unsigned int GLIndexType = 0u;

		if constexpr (std::is_same_v<Buffer<VertexType>::IndType, unsigned int>)
		{
			GLIndexType = GL_UNSIGNED_INT;
		}
		else
		{
			GLIndexType = GL_UNSIGNED_SHORT;
		}
		
		buffer.Bind();
		_shader->Bind();
		_texture->Bind();
		_shader->Uniforms(_uniforms);

		glUniformMatrix4fv(glGetUniformLocation(_shader->ID(), "model"), 1, GL_FALSE, &_model[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(_shader->ID(), "view"), 1, GL_FALSE, &_view[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(_shader->ID(), "projection"), 1, GL_FALSE, &_projection[0][0]);

		glDrawElements(drawMode, buffer.GetNumberOfIndices(), GLIndexType, buffer.Indices());

		buffer.Unbind();
		_shader->Unbind();
		_texture->Unbind();
	}
};

#endif // !PROTOMAPPER_RENDERER_HPP
