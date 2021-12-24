#include "renderer.h"

#include <iostream>

static const char *_src_shader_vertex = ""
	"#version 330 core\n"
	"layout(location = 0) in vec3 pos;\n"
	"layout(location = 1) in vec2 uv;\n"
  "out vec2 vs_uv;\n"
  "uniform mat4 u_mvp;"
	"void main() {\n"
	"	vs_uv = uv;\n"
	"	gl_Position = u_mvp * vec4(pos, 1.0);\n"
	"}";

static const char *_src_shader_fragment = ""
	"#version 330 core\n"
	"out vec4 frag_color;\n"
	"in vec2 vs_uv;\n"
  "uniform sampler2D sampler;\n"
	"void main() {\n"
  " vec4 color = texture2D(sampler, vs_uv);\n"
  " if (color.w == 0)\n"
  "   discard;\n"
	"	frag_color = color;\n"
	"}";

static GLuint compile_shader(GLuint type, const char *src)
{
	int success;
	static char info[1024];
	
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &src, NULL);
	glCompileShader(shader);
	
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
		glGetShaderInfoLog(shader, 1024, NULL, info);
		std::cout << "error: compile_shader():" << std::endl << info << std::endl;
	}
	
	return shader;
}

static GLuint link_shader(GLuint vertex_shader, GLuint fragment_shader)
{
	GLuint program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	
	glLinkProgram(program);
	
	glDetachShader(program, vertex_shader);
	glDetachShader(program, fragment_shader);
	
	return program;
}

shader_t::shader_t()
{
  GLuint vertex = compile_shader(GL_VERTEX_SHADER, _src_shader_vertex);
  GLuint fragment = compile_shader(GL_FRAGMENT_SHADER, _src_shader_fragment);
  
  m_program = link_shader(vertex, fragment);
  m_uloc_mvp = glGetUniformLocation(m_program, "u_mvp");
  
  glDeleteShader(vertex);
  glDeleteShader(fragment);
  
  glUseProgram(m_program);
}

void shader_t::uniform_mvp(const mat4_t &mvp)
{
  glUniformMatrix4fv(m_uloc_mvp, 1, GL_FALSE, mvp.m);
}
