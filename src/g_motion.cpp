#include "game.h"

/*
  Systems for motion/physics. Handles advancing physics state of game and
  resolving collision.
  
  TODO: will document after collision rewrite
*/


// Resolve collisions
void game_t::clip_motion()
{
  for (int i = 0; i < m_num_entities; i++) {
    if (!m_clip[i].is_active() || !m_motion[i].is_active())
        continue;
    
    m_motion[i].velocity *= m_clip[i].velocity_clip;
  }
}

// Advance rigidbody position
void game_t::update_motion()
{
  for (int i = 0; i < m_num_entities; i++) {
    if (!m_motion[i].is_active() || !m_transform[i].is_active())
      continue;
    
    m_transform[i].position += m_motion[i].velocity * m_delta_time;
  }
}
