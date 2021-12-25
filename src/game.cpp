#include "game.h"

#include <iostream>

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
  
  m_sprite[m_player_entity].width = 1;
  m_sprite[m_player_entity].height = 1;
  m_sprite[m_player_entity].state = 1;
  m_sprite[m_player_entity].frame = 0;
  
  // INITIALIZE TEST SPRITE
  entity_t sprite_entity = add_entity();
  m_sprite[sprite_entity].activate();
  m_transform[sprite_entity].activate();
  
  m_sprite[sprite_entity].width = 2;
  m_sprite[sprite_entity].height = 2;
  m_sprite[sprite_entity].state = 5;
  m_sprite[sprite_entity].frame = 0;
  
  m_transform[sprite_entity].rotation = M_PI / 2.0f;
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
  m_client = client;
  m_delta_time = delta_time;
  
  m_time += m_delta_time;
  
  animate_player();
  
  camera_rotate();
  player_move();
  
  apply_friction();
  
  setup_clip();
  clip_map();
  clip_motion();
  update_motion();
  
  update_sprite();
  lock_camera_on_player();
}

void game_t::camera_rotate()
{
  const float CAMERA_ROT_SPEED = 2.0f;
  m_camera.view_angle += m_client.get_rot() * CAMERA_ROT_SPEED * -m_delta_time;
}

void game_t::animate_player()
{
  const float WALK_FRAME_TIME = 0.5f;
  
  float face_dir = constrain_angle(m_transform[m_player_entity].rotation - m_camera.view_angle);
  
  if (face_dir > 1.0f / 6.0f * M_PI && face_dir <= 5.0f / 6.0f * M_PI)
    m_sprite[m_player_entity].state = 1;
  else if (face_dir > 5.0f / 6.0f * M_PI && face_dir <= 7.0f / 6.0f * M_PI)
    m_sprite[m_player_entity].state = 2;
  else if (face_dir > 7.0f / 6.0f * M_PI && face_dir <= 11.0f / 6.0f * M_PI)
    m_sprite[m_player_entity].state = 3;
  else
    m_sprite[m_player_entity].state = 4;
  
  float t = m_time - m_anim[m_player_entity].prev_frame;
  
  if (m_motion[m_player_entity].velocity.length() > 1.0f) {
    if (t > WALK_FRAME_TIME) {
      float over_time = t - WALK_FRAME_TIME;
      m_anim[m_player_entity].prev_frame = m_time + over_time;
      t = over_time;
    }
    
    if (t > WALK_FRAME_TIME / 2.0f)
      m_sprite[m_player_entity].frame = 2;
    else if (t > 0.0f)
      m_sprite[m_player_entity].frame = 1;
  } else {
    m_sprite[m_player_entity].frame = 0;
    m_anim[m_player_entity].prev_frame = m_time;
  }
}

void game_t::player_move()
{
  const float PLAYER_MOVE_ACCEL = 4.0f;
  const float PLAYER_MOVE_SPEED = 24.0f;
  
  if (m_client.get_right() || m_client.get_forward()) {
    vec2_t move_dir = vec2_t(m_client.get_right(), m_client.get_forward()).normalize();
    vec2_t wish_dir = move_dir.rotate(m_camera.view_angle);
    
    float accel = move_accelerate(
      m_motion[m_player_entity].velocity,
      wish_dir,
      PLAYER_MOVE_ACCEL,
      PLAYER_MOVE_SPEED);
    
    m_motion[m_player_entity].velocity += wish_dir * accel;
    m_transform[m_player_entity].rotation = wish_dir.to_angle();
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
        
        // normalize bool between [-1.0, +1.0]
        float x_normal = (depth.x < 0) * 2 - 1;
        float y_normal = (depth.y < 0) * 2 - 1;
        
        plane2d_t plane;
        if (abs(depth.x) < abs(depth.y))
          plane = plane2d_t(vec2_t(x_normal, 0.0f), -abs(depth.x));
        else
          plane = plane2d_t(vec2_t(0.0f, y_normal), -abs(depth.y));
        
        vec2_t adjacent_pos = (map_pos + plane.normal).vround();
        
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
      vec2_t net_normal;
      
      for (int j = 0; j < m_clip[i].num_planes; j++)
        net_normal += m_clip[i].planes[j].normal * -m_clip[i].planes[j].distance;
      
      float beta = 3.0f * -net_normal.length() * m_delta_time;
      float lambda = -(m_motion[i].velocity.dot(net_normal.normalize()) + beta);
      
      if (lambda > 0)
        m_motion[i].velocity += net_normal.normalize() * lambda;
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
  
  m_camera.view_origin = m_transform[m_player_entity].position;
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

