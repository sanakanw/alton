/*

-- game.h --
The main game class designed under an Entity Component System architecture

  Entity: unsigned int used to reference its relevant components
  Component: pure structure which cannot mutate itself
  System: a function which modifies relevant components
  
The game_t class stores components under a fixed size array of MAX_ENTITIES.
Systems iterate from 0 to 'num_entities' and identify entities meet a criteria
of active components. A general system function is structured as:

  void some_system()
  {
    for (int i = 0; i < num_entities; i++) {
      if (!m_some_component[i].is_active() || !m_some_other_component[i].is_active())
        continue;
      
      ...
    }
  }

*/

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

// Static circle collider (NOTE: will soon be deprecated, switching to aabb only collision system)
class circle_t : public component_t
{
public:
  float   radius;
};

// Collision resolution (TODO: fix/rewrite at later date)
class clip_t : public component_t {
public:
  plane_t   planes[MAX_PLANES];
  int       num_planes;
  vec2_t    position;
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

// Raycasting used for continuous collision detecion (NOTE: may be deprecated after collision revamp)
class ray_t {
private:
  bool    m_hit;

public:
  float   distance;
  vec2_t  normal;

public:
  ray_t(const vec2_t &_normal = vec2_t(), float _distance = 0.0f, bool _hit = false)
  {
    m_hit = _hit;
    normal = _normal;
    distance = _distance;
  }
  
  operator bool() const
  {
    return m_hit;
  }
};

// Main game class
class game_t {
private:
  // Input
  client_t    m_client;
  float       m_delta_time;
  
  // Entities
  entity_t    m_entities[MAX_ENTITIES];
  int         m_num_entities;
  
  entity_t add_entity();
  
  // Player Entity
  entity_t    m_player_entity;
  
  // Components
  camera_t    m_camera;
  circle_t    m_circle[MAX_ENTITIES];
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
  void update_motion();
  void clip_motion();
  
  // g_clip.cpp: collision
  void setup_clip();
  void clip_map();
  void clip_circle();
  
  tile_t map_check(int x, int y) const;
  ray_t map_raycast(const vec2_t &pos, const vec2_t &dir, float max_dist) const;
  
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
