#include "game.h"

game_t::game_t()
{
  m_time = 0.0f;
  m_num_entities = 0;
  
  // INITIALIZE PLAYER
  m_player_entity = add_entity();
  m_anim[m_player_entity].activate();
  m_clip[m_player_entity].activate();
  m_sprite[m_player_entity].activate();
  m_motion[m_player_entity].activate();
  m_transform[m_player_entity].activate();
  
  m_sprite[m_player_entity].size = vec2_t(1, 1);
  m_sprite[m_player_entity].offset = vec2_t(0, 1);
  m_sprite[m_player_entity].state = 0;
  m_sprite[m_player_entity].frame = 0;
  
}

void game_t::update(float delta_time, const client_t &client)
{
  m_client = client;
  m_delta_time = delta_time;
  
  m_time += m_delta_time;
  
  animate_player();
  
  camera_rotate();
  player_move();
  
  setup_clip();
  clip_map();
  clip_circle();
  clip_motion();
  update_motion();
  
  update_sprite();
  lock_camera_on_player();
}

void game_t::new_map(mapfile_t &mapfile)
{
  m_map_width = mapfile.get_width();
  m_map_height = mapfile.get_height();
  m_tiles = mapfile.load_tiles();
  
  for (const map_entity_t &map_entity : mapfile.load_entities()) {
    entity_t entity = add_entity();
    switch (map_entity.entity_id - 1) {
    case ENTITY_TREE:
      m_sprite[entity].activate();
      m_transform[entity].activate();
      m_circle[entity].activate();
      
      m_sprite[entity].size = vec2_t(2, 2);
      m_sprite[entity].offset = vec2_t(0, 5);
      m_sprite[entity].state = 0;
      m_sprite[entity].frame = 0;
      
      m_circle[entity].radius = 0.5f;
      
      m_transform[entity].position = map_entity.pos;
      
      break;
    }
  }
}

int game_t::num_entities() const
{
  return m_num_entities;
}

const sprite_t &game_t::get_sprite(entity_t id) const
{
  return m_sprite[id];
}

const camera_t &game_t::get_camera() const
{
  return m_camera;
}

entity_t game_t::add_entity()
{
  return m_num_entities++;
}

