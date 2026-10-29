#pragma once

#include <unordered_map>
#include <string>

#include <glm/glm.hpp>


class ShaderProgram
{
public:
	ShaderProgram();

	void compileVertex  (std::string source);
	void compileFragment(std::string source);

	void link();

	void use() const;
	void cancel() const;

	// TODO-add : cache system
	void uploadMatrix4(std::string key, const glm::mat4& matrix) const;
	void upload1i     (std::string key, int i) const;
	void upload3fv    (std::string key, glm::vec3 vec) const;
	void upload1f     (std::string key, float f) const;

private:	
	uint32_t program_ = 0;

	uint32_t vertexShader_   = 0;
	uint32_t fragmentShader_ = 0;
};
