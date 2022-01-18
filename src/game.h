/*

-- game.h --
The main game class designed under an Entity Component System architecture

  Entity: unsigned int used to reference its relevant components
  Component: pure structure which cannot mutate itself
  System: a function which modifies relevant components

*/

#ifndef GAME_H
#define GAME_H

#include "math3d.h"
#include "mapdefs.h"
#include "mapfile.h"
#include "client.h"
#include <vector>
#include <iostream>

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

// Spatial orientation of entity
class transform_t : public component_t {
public:
  vec2_t  position;
  float   rotation;
};

// Akin to rigidbody; describes an continuous transformation per update
class motion_t : public component_t {
public:
  vec2_t  velocity;
};

// Used for sprite animation
class anim_t : public component_t {
public:
  float   prev_frame;
};

class aabb_t {
public:
  vec2_t  min;
  vec2_t  max;
};

// Static AABB collider
class box_t : public component_t
{
public:
  aabb_t  box;
};

// AABB Collision resolution
class clip_t : public component_t {
public:
  aabb_t  delta_check;
  aabb_t  x_delta_check;
  aabb_t  y_delta_check;
  
  vec2_t  velocity_clip;
};

// Wrapper for an entity which the renderer will use
class sprite_t : public component_t {
public:
  vec2_t  position;
  float   rotation;
  
  vec2_t  size;
  vec2_t  offset;
  
  // frame: x-axis, state: y-axis
  // this can be organised in the sprite map where
  // different states are vertically aranged
  // and the respective animation frames are horizontally aligned
  // this is then simply appended to its texture offset
  int     frame;
  int     state;
};

// Describes spatial orientation of the camera
class camera_t {
public:
  vec2_t  view_origin;
  float   view_angle;
};

// Main game class
class game_t {
private:
  // Input
  client_t    m_client;
  float       m_delta_time;
  float       m_time;
  
  // Entities
  entity_t    m_entities[MAX_ENTITIES];
  int         m_num_entities;
  
  entity_t add_entity();
  
  // Player Entity
  entity_t    m_player_entity;
  
  // Components
  camera_t    m_camera;
  box_t       m_box[MAX_ENTITIES];
  clip_t      m_clip[MAX_ENTITIES];
  anim_t      m_anim[MAX_ENTITIES];
  sprite_t    m_sprite[MAX_ENTITIES];
  motion_t    m_motion[MAX_ENTITIES];
  transform_t m_transform[MAX_ENTITIES];
  
  // Map (TODO: wrap in class)
  int         m_map_width;
  int         m_map_height;
  std::vector<tile_t> m_tiles;
  
  // g_player.cpp: player
  void camera_rotate();
  void player_move();
  void animate_player();
  void lock_camera_on_player();
  
  // g_motion.cpp: physics/motion
  void clip_motion();
  void update_motion();
  
  // g_clip.cpp: collision
  void setup_clip();
  void clip_map();
  void clip_box();
  
  tile_t map_check(int x, int y) const;
  bool aabb_map_check(const aabb_t &box) const;
  
  // g_sprite.cpp: rendering
  void update_sprite();

public:
  game_t();
  
  // Load in a map
  void new_map(mapfile_t &mapfile);
  
  // Advance game state by 'dt' seconds
  void update(float dt, const client_t &client);
  
  // Components necessary for rendering
  int num_entities() const;
  const camera_t &get_camera() const;
  const sprite_t &get_sprite(entity_t id) const;
};

#endif
