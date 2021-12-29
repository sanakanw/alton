#include "game.h"

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

void game_t::update_motion()
{
  for (int i = 0; i < m_num_entities; i++) {
    if (!m_motion[i].is_active() || !m_transform[i].is_active())
      continue;
    
    m_transform[i].position += m_motion[i].velocity * m_delta_time;
  }
}
