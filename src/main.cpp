// GLEW
#include <GL/glew.h>

// SDL
#include <SDL.h>
#include <SDL_opengl.h>

#include <iostream>
#include <sstream>
#include <iomanip>

#include "MyApp.h"

void exitProgram()
{
	SDL_Quit();

	std::cout << "Press Enter to exit..." << std::endl;
	std::cin.get();
}

int main( int argc, char* args[] )
{
	atexit( exitProgram );

	if ( SDL_Init( SDL_INIT_VIDEO ) == -1 )
	{
		std::cout << "[SDL Init]Error while initializing SDL: " << SDL_GetError() << std::endl;
		return 1;
	}
	
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE,         32);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,            8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,          8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,           8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,          8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,		1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,          24);

	SDL_Window *win = 0;
    win = SDL_CreateWindow( "Labirintus", 100, 100, 640,	480, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (win == 0)
	{
		std::cout << "[Window creation]Error while initializing SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

	SDL_GLContext	context	= SDL_GL_CreateContext(win);
    if (context == 0)
	{
		std::cout << "[OGL context creation]Error while initializing SDL: " << SDL_GetError() << std::endl;
        return 1;
    }	

	SDL_GL_SetSwapInterval(1);

	glewExperimental = GL_TRUE; 
	GLenum error = glewInit();
	if ( error != GLEW_OK )
	{
		std::cout << "[GLEW] Error while initializing GLEW!" << std::endl;
		return 1;
	}

	int glVersion[2] = {-1, -1}; 
	glGetIntegerv(GL_MAJOR_VERSION, &glVersion[0]); 
	glGetIntegerv(GL_MINOR_VERSION, &glVersion[1]); 
	std::cout << "Running OpenGL " << glVersion[0] << "." << glVersion[1] << std::endl;

	if ( glVersion[0] == -1 && glVersion[1] == -1 )
	{
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow( win );

		std::cout << "[OGL context creation] Could not create OpenGL context! Maybe bad SDL_GL_SetAttribute(...) call." << std::endl;

		return 1;
	}

	std::stringstream window_title;
	window_title << "OpenGL " << glVersion[0] << "." << glVersion[1];
	SDL_SetWindowTitle(win, window_title.str().c_str());


	bool quit = false;
	SDL_Event ev;
	
	CMyApp app;
	if (!app.Init())
	{
		SDL_DestroyWindow(win);
		std::cout << "[app.Init] Error while initializing application" << std::endl;
		return 1;
	}

	unsigned t = SDL_GetTicks();
	while (!quit)
	{
		while ( SDL_PollEvent(&ev) )
		{
			switch (ev.type)
			{
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_KEYDOWN:
				if ( ev.key.keysym.sym == SDLK_ESCAPE )
					quit = true;
				app.KeyboardDown(ev.key);
				break;
			case SDL_KEYUP:
				app.KeyboardUp(ev.key);
				break;
			case SDL_MOUSEBUTTONDOWN:
				app.MouseDown(ev.button);
				break;
			case SDL_MOUSEBUTTONUP:
				app.MouseUp(ev.button);
				break;
			case SDL_MOUSEWHEEL:
				app.MouseWheel(ev.wheel);
				break;
			case SDL_MOUSEMOTION:
				app.MouseMove(ev.motion);
				break;
			case SDL_WINDOWEVENT:
				if ( ev.window.event == SDL_WINDOWEVENT_SIZE_CHANGED )
				{
					app.Resize(ev.window.data1, ev.window.data2);
				}
				break;
			}
		}

		app.Update();
		app.Render();

		SDL_GL_SwapWindow(win);
		
		unsigned spf = SDL_GetTicks() - t;
		float fps = 1000.0/spf;
		std::stringstream window_title;
		window_title << std::setprecision(2) << "OpenGL " << glVersion[0] << "." << glVersion[1] << " " << fps << " FPS";
		SDL_SetWindowTitle(win, window_title.str().c_str());
		t = SDL_GetTicks();
	}


	app.Clean();

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow( win );

	return 0;
}
