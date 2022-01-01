#include <iostream>

#include "win.h"
#include "game.h"
#include "renderer.h"
#include "log.h"

#define WIDTH							1280
#define HEIGHT						960
#define ASPECT_RATIO			((float) HEIGHT / (float) WIDTH)

#define SECONDS_PER_TICK	1.0f / 60.0f

int main(int argc, char **argv)
{
  win_api_init();
  win_t win(WIDTH, HEIGHT);
	
	gl_api_init();
  
  game_t game;
  client_t client;
	
  renderer_t renderer(game, ASPECT_RATIO);
  
  std::ifstream file_map_map("map.map");
  
  mapfile_t mapfile = mapfile_t(file_map_map);
  game.new_map(mapfile);
  renderer.new_map(mapfile);
  
  float prev_time = win_get_time();
	float cur_time = prev_time;
	float unprocessed_time = 0.0f;
  
  while (!win.is_quit()) {
    cur_time = win_get_time();
    float dt = cur_time - prev_time;
    prev_time = cur_time;
		
		unprocessed_time += dt;
    
		if (unprocessed_time >= SECONDS_PER_TICK) {
      unprocessed_time -= SECONDS_PER_TICK;
      
      win.poll(client);
      game.update(SECONDS_PER_TICK, client);
			renderer.render_player_view();
			win.swap();
		}
  };
	
  win.quit();
  
  return 0;
}
