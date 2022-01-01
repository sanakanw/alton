#include "win.h"

#include <SDL2/SDL.h>
#include "log.h"

static SDL_Window *sdl_window;
static SDL_GLContext sdl_gl_context;

void win_api_init()
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  
  // SDL_SetRelativeMouseMode(SDL_TRUE);
}

float win_get_time()
{
  return SDL_GetTicks() / 1000.0f;
}

win_t::win_t(int width, int height)
{
  sdl_window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	sdl_gl_context = SDL_GL_CreateContext(sdl_window);
	
	if (!sdl_gl_context)
		LOG_ERROR("win_t::win_t()") << "failed to create GL context: " << SDL_GetError();
}

void win_t::poll(client_t &client)
{
  static SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_KEYDOWN:
      client.key_event(event.key.keysym.sym, 1);
      break;
    case SDL_KEYUP:
      client.key_event(event.key.keysym.sym, 0);
      break;
    case SDL_MOUSEMOTION:
      client.mouse_move(event.motion.xrel, event.motion.yrel);
      break;
    case SDL_QUIT:
      m_quit = true;
      break;
    }
  }
}

void win_t::quit()
{
  SDL_GL_DeleteContext(sdl_gl_context);
  SDL_DestroyWindow(sdl_window);
  SDL_Quit();
}

void win_t::swap()
{
  SDL_GL_SwapWindow(sdl_window);
}

bool win_t::is_quit()
{
  return m_quit;
}
