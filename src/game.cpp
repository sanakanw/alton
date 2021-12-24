#include "game.h"

#include <iostream>

game_t::game_t()
{
  m_tick = 0;
  m_num_entities = 0;
  
  m_player_entity = add_entity();
  m_sprite[m_player_entity].activate();
  m_transform[m_player_entity].activate();
  m_clip[m_player_entity].activate();
  m_motion[m_player_entity].activate();
  m_sprite[m_player_entity].state = 1;
}

float game_t::move_accelerate(const vec2_t &prev_velocity, const vec2_t &wish_dir, float accel, float wish_speed) const
{
  float current_speed = prev_velocity.dot(wish_dir);
  float add_speed = wish_speed - current_speed;
  
  if (add_speed <= 0)
    return 0;
  
  float accel_speed = accel * wish_speed * m_delta_time;
  
  if (accel_speed > add_speed)
    accel_speed = add_speed;
  
  return accel_speed;
}

tile_t game_t::map_check(int x, int y) const
{
  if (x < 0 || y < 0 || x >= m_map_width || y >= m_map_height)
    return TILE_SNOW;
  
  return m_tiles[x + y * m_map_width];
}

void game_t::update(float delta_time, const client_t &client)
{
  m_tick++;
  
  m_client = client;
  m_delta_time = delta_time;
  
  player_look();
  player_move();
  
  apply_friction();
  
  setup_clip();
  clip_map();
  clip_motion();
  update_motion();
  
  update_sprite();
  lock_camera_on_player();
}

void game_t::player_look()
{
  m_transform[m_player_entity].rotation += m_client.get_rot() * -m_delta_time * 2;
}

void game_t::player_move()
{
  const float PLAYER_MOVE_SPEED = 24.0f;
  
  vec2_t move_dir = vec2_t(m_client.get_right(), m_client.get_forward()).normalize();
  
  if (move_dir.length() > 0.1f) {
    vec2_t wish_dir = move_dir.rotate(m_transform[m_player_entity].rotation);
    
    float accel = move_accelerate(m_motion[m_player_entity].velocity, wish_dir, 4.0f, PLAYER_MOVE_SPEED);
  
    m_sprite[m_player_entity].frame = 1 + ((m_tick % 30) > 15);
    m_motion[m_player_entity].velocity += wish_dir * accel;
  } else {
    m_sprite[m_player_entity].frame = 0;
  }
}

void game_t::setup_clip()
{
  for (int i = 0; i < m_num_entities; i++) {
    if (!m_clip[i].is_active() || !m_motion[i].is_active() || !m_transform[i].is_active())
      continue;
    
    m_clip[i].position = m_transform[i].position + m_motion[i].velocity * m_delta_time;
  }
}

void game_t::clip_map()
{
  static const vec2_t box_hull[] = {
    vec2_t(-0.4f, -0.4f),
    vec2_t(+0.4f, -0.4f),
    vec2_t(-0.4f, +0.4f),
    vec2_t(+0.4f, +0.4f),
  };
  
  static const int num_box_hull = sizeof(box_hull) / sizeof(vec2_t);
  
  for (int i = 0; i < m_num_entities; i++) {
    if (!m_clip[i].is_active())
      continue;
    
    m_clip[i].num_planes = 0;
    for (int j = 0; j < num_box_hull; j++) {
      vec2_t check_pos = m_clip[i].position + box_hull[j];
      vec2_t map_pos = check_pos.vfloor();
      
      if (map_check(map_pos.x, map_pos.y) == TILE_WALL) {
        vec2_t corner_pos = check_pos.vround();
        vec2_t depth = check_pos - corner_pos;
        
        plane_t plane;
        if (abs(depth.x) < abs(depth.y))
          plane = plane_t(vec3_t((depth.x < 0) * 2 - 1, 0.0f, 0.0f), -abs(depth.x));
        else
          plane = plane_t(vec3_t(0.0f, (depth.y < 0) * 2 - 1, 0.0f), -abs(depth.y));
        
        vec2_t adjacent_pos = (map_pos + plane.normal.xy()).vround();
        
        if (map_check(adjacent_pos.x, adjacent_pos.y) != TILE_WALL) {
          m_clip[i].planes[m_clip[i].num_planes] = plane;
          m_clip[i].num_planes++;
        }
      }
    }
  }
}

void game_t::clip_motion()
{
  
  for (int i = 0; i < m_num_entities; i++) {
    if (!m_clip[i].is_active() || !m_motion[i].is_active())
        continue;
    
    if (m_clip[i].num_planes > 0) {
      vec3_t net_normal;
      
      for (int j = 0; j < m_clip[i].num_planes; j++)
        net_normal += m_clip[i].planes[j].normal * -m_clip[i].planes[j].distance;
      
      float beta = 3.0f * -net_normal.length() * m_delta_time;
      float lambda = -(m_motion[i].velocity.dot(net_normal.normalize().xy()) + beta);
      
      if (lambda > 0)
        m_motion[i].velocity += net_normal.normalize().xy() * lambda;
    }
  }
}

void game_t::apply_friction()
{
  static const float MOTION_FRICTION = 16.0f;
  
  for (int i = 0; i < m_num_entities; i++) {
    if (!m_motion[i].is_active())
      continue;
    
    float speed = m_motion[i].velocity.length();
    
    if (speed != 0) {
      float drop = speed * MOTION_FRICTION * m_delta_time;
      m_motion[i].velocity *= std::max((speed - drop), 0.0f) / speed;
    }
  }
}

void game_t::update_sprite()
{
  for (int i = 0; i < m_num_entities; i++) {
    if (!m_sprite[i].is_active() || !m_transform[i].is_active())
      continue;
    
    m_sprite[i].position = m_transform[i].position;
    m_sprite[i].rotation = m_transform[i].rotation;
  }
}

void game_t::update_motion()
{
  for (int i = 0; i < m_num_entities; i++) {
    if (!m_motion[i].is_active() || !m_transform[i].is_active())
      continue;
    
    m_transform[i].position += m_motion[i].velocity * m_delta_time;
  }
}

void game_t::lock_camera_on_player()
{
  if (!m_transform[m_player_entity].is_active())
    return;
  
  m_camera.view_origin = vec3_t(m_transform[m_player_entity].position, 0);
}

void game_t::new_map(mapfile_t &mapfile)
{
  m_map_width = mapfile.get_width();
  m_map_height = mapfile.get_height();
  m_tiles = mapfile.load_tiles();
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

