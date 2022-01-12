#include "game.h"

void game_t::clip_motion()
{
  for (int i = 0; i < m_num_entities; i++) {
    if (!m_clip[i].is_active() || !m_motion[i].is_active())
        continue;
    
    if (m_clip[i].num_planes > 0) {
      vec2_t net_normal;
      
      for (int j = 0; j < m_clip[i].num_planes; j++) {
        float beta = 3.0f * m_clip[i].planes[j].distance * m_delta_time;
        float lambda = -(m_motion[i].velocity.dot(m_clip[i].planes[j].normal) + beta);
        
        if (lambda < 0)
          m_motion[i].velocity += m_clip[i].planes[j].normal * lambda;
      }
    }
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
