#ifndef GAME_H
#define GAME_H

#include "math3d.h"
#include "mapdefs.h"
#include "mapfile.h"
#include "client.h"
#include <vector>

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

class clip_t : public component_t {
public:
  vec2_t    position;
  int       num_planes;
  plane2d_t planes[MAX_PLANES];
};

class sprite_t : public component_t {
public:
  vec2_t  position;
  float   rotation;
  int     width;
  int     height;
  int     state;
  int     frame;
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
  int         m_tick;
  
  entity_t    m_player_entity;
  
  camera_t    m_camera;
  transform_t m_transform[MAX_ENTITIES];
  sprite_t    m_sprite[MAX_ENTITIES];
  clip_t      m_clip[MAX_ENTITIES];
  motion_t    m_motion[MAX_ENTITIES];
  
  
  int         m_map_width;
  int         m_map_height;
  std::vector<tile_t> m_tiles;
  
  tile_t map_check(int x, int y) const;
  float move_accelerate(const vec2_t &prev_velocity, const vec2_t &wish_dir, float accel, float wish_speed) const;
  
  void camera_rotate();
  void player_move();
  void apply_friction();
  void update_sprite();
  void update_motion();
  void setup_clip();
  void clip_map();
  void clip_motion();
  void lock_camera_on_player();
  
  entity_t add_entity();

public:
  game_t();
  
  void new_map(mapfile_t &mapfile);
  
  void update(float dt, const client_t &client);
  
  int num_entities() const;
  
  const camera_t &get_camera() const;
  const sprite_t &get_sprite(entity_t id) const;
};

#endif
