#pragma once

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include <GL/glew.h>

#include <SDL_image.h>
#include <SDL_ttf.h>

#if defined (__linux__) && defined (__INSTALL)
#define PATH std::string("/usr/share/labirintus/")
#else
#define PATH
#endif

/* 

According to http://www.opengl-tutorial.org/

*/
GLuint loadShader(GLenum _shaderType, const char* _fileName)
{
	GLuint loadedShader = glCreateShader( _shaderType );

	if ( loadedShader == 0 )
	{
		fprintf(stderr, "Error while initalizing %s shader (glCreateShader)!", _fileName);
		return 0;
	}
	
	std::string shaderCode = "";

	std::ifstream shaderStream(_fileName);

	if ( !shaderStream.is_open() )
	{
		fprintf(stderr, "Error while loading %s shader (glCreateShader)!", _fileName);
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
		std::vector<char> VertexShaderErrorMessage(infoLogLength);
		glGetShaderInfoLog(loadedShader, infoLogLength, NULL, &VertexShaderErrorMessage[0]);

		fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);
	}

	return loadedShader;
}

GLuint loadProgramVSGSFS(const char* _fileNameVS, const char* _fileNameGS, const char* _fileNameFS)
{
	GLuint vs_ID = loadShader(GL_VERTEX_SHADER,		_fileNameVS);
	GLuint gs_ID = loadShader(GL_GEOMETRY_SHADER,	_fileNameGS);
	GLuint fs_ID = loadShader(GL_FRAGMENT_SHADER,	_fileNameFS);

	if ( vs_ID == 0 || gs_ID == 0 || fs_ID == 0 )
	{
		return 0;
	}

	GLuint program_ID = glCreateProgram();

	fprintf(stdout, "Linking program\n");
	glAttachShader(program_ID, vs_ID);
	glAttachShader(program_ID, gs_ID);
	glAttachShader(program_ID, fs_ID);

	glLinkProgram(program_ID);

	GLint infoLogLength = 0, result = 0;

	glGetProgramiv(program_ID, GL_LINK_STATUS, &result);
	glGetProgramiv(program_ID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if ( GL_FALSE == result)
	{
		std::vector<char> ProgramErrorMessage( infoLogLength );
		glGetProgramInfoLog(program_ID, infoLogLength, NULL, &ProgramErrorMessage[0]);
		fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
	}

	glDeleteShader( vs_ID );
	glDeleteShader( gs_ID );
	glDeleteShader( fs_ID );

	return program_ID;
}

GLuint TextureFromFile(const std::string& filename)
{
	SDL_Surface* loaded_img = IMG_Load(filename.c_str());  

	int img_mode = 0;
	
	if ( loaded_img == 0 )
	{
		std::cout << "[TextureFromFile] Error while loading image: " << filename << std::endl;
		return 0;
	}
	// TODO: fix this (can't be determined compile time)
	#if SDL_BYTEORDER == SDL_LIL_ENDIAN
		if ( loaded_img->format->BytesPerPixel == 4 )
			img_mode = GL_BGRA;
		else
			img_mode = GL_BGR;
	#else
		if ( loaded_img->format->BytesPerPixel == 4 )
			img_mode = GL_RGBA;
		else
			img_mode = GL_RGB;
	#endif

    GLuint tex;
    glGenTextures(1, &tex);

    glBindTexture(GL_TEXTURE_2D, tex);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, loaded_img->w, loaded_img->h, img_mode, GL_UNSIGNED_BYTE, loaded_img->pixels);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	SDL_FreeSurface( loaded_img );

    return tex;
}

GLuint PointtableFromFile(const std::string& filename, int coins, int diamonds)
{
	SDL_Surface* loaded_img = IMG_Load(filename.c_str());

	int img_mode = 0;
	
	if ( loaded_img == 0 )
	{
		std::cout << "[TextureFromFile] Error while loading image: " << filename << std::endl;
		return 0;
	}

	#if SDL_BYTEORDER != SDL_LIL_ENDIAN
		if ( loaded_img->format->BytesPerPixel == 4 )
			img_mode = GL_BGRA;
		else
			img_mode = GL_BGR;
	#else
		if ( loaded_img->format->BytesPerPixel == 4 )
			img_mode = GL_RGBA;
		else
			img_mode = GL_RGB;
	#endif
	
	std::string fontfile = PATH+"textures/font.ttf";
	TTF_Font* font = TTF_OpenFont(fontfile.c_str(), 50);
	SDL_Surface* text1 = TTF_RenderText_Solid(font, std::to_string(coins).c_str(), {0,0,0});
	SDL_Surface* text2 = TTF_RenderText_Solid(font, std::to_string(diamonds).c_str(), {0,0,0});
	
	SDL_Rect renderQuad = { 100, 17, 40, 40 };
	SDL_BlitSurface(text1, 0, loaded_img, &renderQuad);
	renderQuad = { 100, 77, 40, 40 };
	SDL_BlitSurface(text2, 0, loaded_img, &renderQuad);
	
    GLuint tex;
    glGenTextures(1, &tex);

    glBindTexture(GL_TEXTURE_2D, tex);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, loaded_img->w, loaded_img->h, img_mode, GL_UNSIGNED_BYTE, loaded_img->pixels);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	SDL_FreeSurface( loaded_img );

    return tex;
}
