#ifndef GAME_H
#define GAME_H

#include "math3d.h"
#include "mapdefs.h"
#include "mapfile.h"
#include "client.h"
#include <vector>
#include <iostream>

#define MAX_PLANES 8
#define MAX_ENTITIES 256

typedef unsigned int entity_t;

class component_t {
private:
  bool m_active;

public:
  component_t()
  {
    m_active = false;
  }
  
  bool is_active() const
  {
    return m_active;
  }
  
  void activate()
  {
    m_active = true;
  }
  
  void deactivate()
  {
    m_active = false;
  }
};

class transform_t : public component_t {
public:
  vec2_t  position;
  float   rotation;
};

class motion_t : public component_t {
public:
  vec2_t  velocity;
};

class anim_t : public component_t {
public:
  float   prev_frame;
};

class circle_t : public component_t
{
public:
  float   radius;
};

class clip_t : public component_t {
public:
  plane2d_t planes[MAX_PLANES];
  int       num_planes;
  vec2_t    position;
};

class sprite_t : public component_t {
public:
  vec2_t  position;
  float   rotation;
  
  vec2_t  size;
  vec2_t  offset;
  
  int     frame;
  int     state;
};

class camera_t {
public:
  vec2_t  view_origin;
  float   view_angle;
};

class game_t {
private:
  entity_t    m_entities[MAX_ENTITIES];
  int         m_num_entities;
  
  client_t    m_client;
  float       m_delta_time;
  float       m_time;
  
  entity_t    m_player_entity;
  
  camera_t    m_camera;
  circle_t      m_circle[MAX_ENTITIES];
  clip_t      m_clip[MAX_ENTITIES];
  anim_t      m_anim[MAX_ENTITIES];
  sprite_t    m_sprite[MAX_ENTITIES];
  motion_t    m_motion[MAX_ENTITIES];
  transform_t m_transform[MAX_ENTITIES];
  
  int         m_map_width;
  int         m_map_height;
  std::vector<tile_t> m_tiles;
  
  entity_t add_entity();
  
  // g_player.cpp
  void camera_rotate();
  void player_move();
  void animate_player();
  void lock_camera_on_player();
  
  // g_motion.cpp
  void apply_friction();
  void update_motion();
  void clip_motion();
  
  float move_accelerate(const vec2_t &prev_velocity, const vec2_t &wish_dir, float accel, float wish_speed) const;
  
  // g_clip.cpp
  void setup_clip();
  void clip_map();
  void clip_circle();
  
  tile_t map_check(int x, int y) const;
  
  // g_sprite.cpp
  void update_sprite();

public:
  game_t();
  
  void new_map(mapfile_t &mapfile);
  
  void update(float dt, const client_t &client);
  
  int num_entities() const;
  
  const camera_t &get_camera() const;
  const sprite_t &get_sprite(entity_t id) const;
};

#endif
