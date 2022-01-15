#include "game.h"

/*
  TODO: will document after rewrite
*/

tile_t game_t::map_check(int x, int y) const
{
  if (x < 0 || y < 0 || x >= m_map_width || y >= m_map_height)
    return TILE_SNOW;
  
  return m_tiles[x + y * m_map_width];
}

ray_t game_t::map_raycast(const vec2_t &pos, const vec2_t &dir, float max_dist) const
{
  const vec2_t delta_dist = vec2_t(abs(1.0f / dir.x), abs(1.0f / dir.y));
  
  vec2_t map_pos = pos.vfloor();
  vec2_t side_dist, step;
  
  if (dir.x < 0) {
    step.x = -1;
    side_dist.x = (pos.x - map_pos.x) * delta_dist.x;
  } else {
    step.x = +1;
    side_dist.x = (map_pos.x + 1 - pos.x) * delta_dist.x;
  }
  
  if (dir.y < 0) {
    step.y = -1;
    side_dist.y = (pos.y - map_pos.y) * delta_dist.y;
  } else {
    step.y = +1;
    side_dist.y = (map_pos.y + 1 - pos.y) * delta_dist.y;
  }
  
  bool side = false;
  while (map_check((int) map_pos.x, (int) map_pos.y) != TILE_WALL) {
    if (side_dist.x < side_dist.y) {
      if (side_dist.x > max_dist)
        return ray_t();
      
      side = true;
      side_dist.x += delta_dist.x;
      map_pos.x += step.x;
    } else {
      if (side_dist.y > max_dist)
        return ray_t();
      
      side = false;
      side_dist.y += delta_dist.y;
      map_pos.y += step.y;
    }
  }
  
  if (side)
    return ray_t(vec2_t(step.x, 0), side_dist.x - delta_dist.x, true);
  else
    return ray_t(vec2_t(0, step.y), side_dist.y - delta_dist.y, true);
}

void game_t::setup_clip()
{
  for (int i = 0; i < m_num_entities; i++) {
    if (!m_motion[i].is_active()
    || !m_clip[i].is_active()
    || !m_transform[i].is_active())
      continue;
    
    m_clip[i].position = m_transform[i].position + m_motion[i].velocity * m_delta_time;
  }
}

void game_t::clip_circle()
{
  static const float clip_radius = 0.4f;
  
  for (int i = 0; i < m_num_entities; i++) {
    if (!m_clip[i].is_active())
      continue;
    
    for (int j = 0; j < m_num_entities; j++) {
      if (!m_circle[j].is_active() || !m_transform[j].is_active())
        continue;
      
      vec2_t delta = m_transform[j].position - m_clip[i].position;
      float delta_distance = delta.length() - m_circle[j].radius - clip_radius;
      
      if (delta_distance < 0) {
        m_clip[i].planes[m_clip[i].num_planes] = plane_t(delta.normalize(), delta_distance);
        m_clip[i].num_planes++;
      }
    }
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
    if (!m_clip[i].is_active() || !m_transform[i].is_active())
      continue;
    
    const vec2_t delta_dist = m_clip[i].position - m_transform[i].position;
    const vec2_t ray_dir = delta_dist.normalize();
    
    m_clip[i].num_planes = 0;
    for (int j = 0; j < num_box_hull; j++) {
      const vec2_t check_pos = m_transform[i].position + box_hull[j];
      const ray_t hit = map_raycast(check_pos, ray_dir, 1.0f);
      
      if (hit && hit.distance <= delta_dist.length()) {
        bool is_duplicate = false;
        for (int k = 0; k < m_clip[i].num_planes; k++) {
          if (m_clip[i].planes[k].normal.dot(hit.normal) > 0.9) {
            is_duplicate = true;
            break;
          }
        }
        
        if (!is_duplicate) {
          m_clip[i].planes[m_clip[i].num_planes].normal = hit.normal;
          m_clip[i].planes[m_clip[i].num_planes].distance = delta_dist.length() - hit.distance;
          m_clip[i].num_planes++;
        }
      }
    }
  }
}
