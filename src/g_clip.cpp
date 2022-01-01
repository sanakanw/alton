#include "game.h"

tile_t game_t::map_check(int x, int y) const
{
  if (x < 0 || y < 0 || x >= m_map_width || y >= m_map_height)
    return TILE_SNOW;
  
  return m_tiles[x + y * m_map_width];
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
      
      vec2_t delta = m_clip[i].position - m_transform[j].position;
      float delta_distance = delta.length() - m_circle[j].radius - clip_radius;
      
      if (delta_distance < 0) {
        m_clip[i].planes[m_clip[i].num_planes] = plane2d_t(delta.normalize(), delta_distance);
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
