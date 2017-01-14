#include "gShaderProgram.h"

#include <iostream>
#include <fstream>
#include <vector>

gShaderProgram::gShaderProgram(void) : m_map_uniform_locations(), m_list_shaders_attached(), m_id_program(0)
{
}


gShaderProgram::~gShaderProgram(void)
{
}

bool gShaderProgram::AttachShader( GLenum _shaderType,  const char* _filename )
{
	GLuint loaded_shader = loadShader(_shaderType, _filename);
	if ( loaded_shader == 0 )
		return false;

	m_list_shaders_attached.push_back( loaded_shader );

	if ( m_id_program == 0 )
	{
		m_id_program = glCreateProgram();
		if ( m_id_program == 0 )
		{
			if ( m_verbose )
			{
				std::cout << "Hiba a shader program létrehozásakor" << std::endl;
			}
			return false;
		}
	}

	glAttachShader( m_id_program, loaded_shader );

	return true;
}

bool gShaderProgram::LinkProgram()
{
	if ( m_id_program == 0 )
		return false;

	glLinkProgram(m_id_program);

	GLint infoLogLength = 0, result = 0;

	glGetProgramiv(m_id_program, GL_LINK_STATUS, &result);
	glGetProgramiv(m_id_program, GL_INFO_LOG_LENGTH, &infoLogLength);
	if ( GL_FALSE == result)
	{
		if ( m_verbose )
		{
			std::vector<char> ProgramErrorMessage( infoLogLength );
			glGetProgramInfoLog(m_id_program, infoLogLength, NULL, &ProgramErrorMessage[0]);
			fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
		}
		return false;
	}

	return true;
}

void gShaderProgram::Clean()
{
	for (	std::list<GLuint>::iterator _it = m_list_shaders_attached.begin();
			_it != m_list_shaders_attached.end();
			++_it )
	{
		if ( m_id_program != 0 && *_it != 0)
			glDetachShader( m_id_program, *_it );
		if ( *_it != 0 )
			glDeleteShader( *_it );
	}
	glDeleteProgram(m_id_program);
}

void gShaderProgram::SetVerbose( bool _val )
{
	m_verbose = _val;
}

GLuint gShaderProgram::loadShader(GLenum _shaderType, const char* _fileName)
{
	GLuint loadedShader = glCreateShader( _shaderType );

	if ( loadedShader == 0 )
	{
		if (m_verbose)
			fprintf(stderr, "Hiba a %s shader inicializálásakor (glCreateShader)!", _fileName);
		return 0;
	}
	
	std::string shaderCode = "";

	std::ifstream shaderStream(_fileName);

	if ( !shaderStream.is_open() )
	{
		if (m_verbose)
			fprintf(stderr, "Hiba a %s shader fájl betöltésekor!", _fileName);
		return 0;
	}

	std::string line = "";
	while ( std::getline(shaderStream, line) )
	{
		shaderCode += line + "\n";
	}

	shaderStream.close();

	const char* sourcePointer = shaderCode.c_str();
	glShaderSource( loadedShader, 1, &sourcePointer, NULL );

	glCompileShader( loadedShader );

	GLint result = GL_FALSE;
    int infoLogLength;

	glGetShaderiv(loadedShader, GL_COMPILE_STATUS, &result);
	glGetShaderiv(loadedShader, GL_INFO_LOG_LENGTH, &infoLogLength);

	if ( GL_FALSE == result )
	{
		if (m_verbose)
		{
			std::vector<char> VertexShaderErrorMessage(infoLogLength);
			glGetShaderInfoLog(loadedShader, infoLogLength, NULL, &VertexShaderErrorMessage[0]);

			std::cout << "Error: " << &VertexShaderErrorMessage[0] << std::endl;
		}
		loadedShader = 0;
	}

	return loadedShader;
}

void gShaderProgram::BindAttribLoc(int _index, const char* _uniform)
{
	glBindAttribLocation(m_id_program, _index, _uniform);
}

void gShaderProgram::BindFragDataLoc(int _index, const char* _uniform)
{
	glBindFragDataLocation(m_id_program, _index, _uniform);
}

void gShaderProgram::SetUniform(const char* _uniform, int _i)
{
	GLint loc = getLocation(_uniform);
	glUniform1i( loc, _i );
}

void gShaderProgram::SetUniform(const char* _uniform, float _f)
{
	GLint loc = getLocation(_uniform);
	glUniform1f( loc, _f );
}

void gShaderProgram::SetUniform(const char* _uniform, float _a, float _b)
{
	GLint loc = getLocation(_uniform);
	glUniform2f( loc, _a, _b );
}

void gShaderProgram::SetUniform(const char* _uniform, float _a, float _b, float _c)
{
	GLint loc = getLocation(_uniform);
	glUniform3f( loc, _a, _b, _c );
}

void gShaderProgram::SetUniform(const char* _uniform, float _a, float _b, float _c, float _d)
{
	GLint loc = getLocation(_uniform);
	glUniform4f( loc, _a, _b, _c, _d );
}

void gShaderProgram::SetUniform(const char* _uniform, const glm::vec2& _vec)
{
	GLint loc = getLocation(_uniform);
	glUniform2fv( loc, 1, &_vec[0] );
}

void gShaderProgram::SetUniform(const char* _uniform, const glm::vec3& _vec)
{
	GLint loc = getLocation(_uniform);
	glUniform3fv( loc, 1, &_vec[0] );
}

void gShaderProgram::SetUniform(const char* _uniform, const glm::vec4& _vec)
{
	GLint loc = getLocation(_uniform);
	glUniform4fv( loc, 1, &_vec[0] );
}

void gShaderProgram::SetUniform(const char* _uniform, const glm::mat4& _mat)
{
	GLint loc = getLocation(_uniform);
	glUniformMatrix4fv( loc, 1, GL_FALSE, &(_mat[0][0]) );
}

inline GLuint	gShaderProgram::getLocation(const char* _uniform)
{
	std::map< std::string, int >::iterator loc_it = m_map_uniform_locations.find(_uniform);
	if ( loc_it == m_map_uniform_locations.end() )
	{
		GLint my_loc = glGetUniformLocation(m_id_program, _uniform);
		m_map_uniform_locations[_uniform] = my_loc;
		return my_loc;
	}
	else
		return loc_it->second;
}

void gShaderProgram::On()
{
	glUseProgram(m_id_program);
}

void gShaderProgram::Off()
{
	glUseProgram(0);
}

void gShaderProgram::SetTexture(const char* _uniform, int _sampler, GLuint _textureID)
{
	glActiveTexture(GL_TEXTURE0 + _sampler);
	glBindTexture(GL_TEXTURE_2D, _textureID);
	SetUniform(_uniform, _sampler);
}

void gShaderProgram::SetCubeTexture(const char* _uniform, int _sampler, GLuint _textureID)
{
	glActiveTexture(GL_TEXTURE0 + _sampler);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _textureID);
	SetUniform(_uniform, _sampler);
}
