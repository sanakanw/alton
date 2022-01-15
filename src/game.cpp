#include "game.h"

game_t::game_t()
{
  m_num_entities = 0;
  
  // components should be initialized by allocating an
  // entity id using add_entity()
  // its relevant components should then be activated using specified id
  // initial component states can be then be initialized
  
  // initialize the player
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
  
  // logical order of systems:
  // systems updating components based on player input
  // systems advancing game state: motion
  // systems updating visual components
  
  camera_rotate();
  player_move();
  
  setup_clip();
  clip_map();
  clip_circle();
  clip_motion();
  update_motion();
  
  animate_player();
  update_sprite();
  lock_camera_on_player();
}

// Kinda dodgy; map handling rewrite coming soon
void game_t::new_map(mapfile_t &mapfile)
{
  m_map_width = mapfile.get_width();
  m_map_height = mapfile.get_height();
  m_tiles = mapfile.load_tiles(); // array of unsigned integers
  
  // load map_entity from map: position and entity id
  // based off the entity id, create the entity in game_t
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

