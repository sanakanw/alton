#include <iostream>

#include "win.h"
#include "game.h"
#include "renderer.h"

int main(int argc, char **argv)
{
  win_t::init();
  
  win_t win(1280, 960);
  
  game_t game;
  client_t client;
  renderer_t renderer(game, 960.0f/1280.0f);
  
  float prev_time, cur_time;
  
  std::ifstream file_map_map("map.map");
  
  mapfile_t mapfile = mapfile_t(file_map_map);
  game.new_map(mapfile);
  renderer.new_map(mapfile);
  
  prev_time = win_t::get_time();
  
  while (!win.is_quit()) {
    win.poll(client);
    
    cur_time = win_t::get_time();
    float dt = cur_time - prev_time;
    prev_time = cur_time;
    
    game.update(dt, client);
    
    renderer.render_player_view();
    
    win.swap();
  }
  
  win.quit();
  
  return 0;
}
