#include "game.h"

/*
  Collision detection using exclusively AABB
  
  Collision resolution is simply detecting whether if the new position
  collides. If it does, check if it can move on any of the axes without
  colliding. If it cannot, then clip the velocity on that axis.
*/

static const vec2_t clip_box_size = vec2_t(0.4, 0.4);  

tile_t game_t::map_check(int x, int y) const
{
  if (x < 0 || y < 0 || x >= m_map_width || y >= m_map_height)
    return TILE_SNOW;
  
  return m_tiles[x + y * m_map_width];
}

// by subtracting the minimum points with the maximum points
// if it is colliding, on any axis, the value should be negative
bool aabb_box_check(const aabb_t &box1, const aabb_t &box2)
{
  vec2_t dist1 = box1.min - box2.max;
  vec2_t dist2 = box2.min - box1.max;
  
  return dist1.x < 0.0f && dist1.y < 0.0f && dist2.x < 0.0f && dist2.y < 0.0f;
}

// sweep across the tiles which the bounding box covers
bool game_t::aabb_map_check(const aabb_t &box) const
{
  int x0 = (int) floor(box.min.x);
  int x1 = (int) floor(box.max.x);
  
  int y0 = (int) floor(box.min.y);
  int y1 = (int) floor(box.max.y);
  
  for (int x = x0; x <= x1; x++) {
    for (int y = y0; y <= y1; y++) {
      if (map_check(x, y) == TILE_WALL)
        return true;
    }
  }
  
  return false;
}

void game_t::setup_clip()
{
  for (int i = 0; i < m_num_entities; i++) {
    if (!m_clip[i].is_active() || !m_transform[i].is_active() || !m_motion[i].is_active())
      continue;
    
    // reset velocity clip
    m_clip[i].velocity_clip = vec2_t(1.0f, 1.0f);
    
    // predict the entity's moment in this frame
    vec2_t delta_pos = m_motion[i].velocity * m_delta_time;
    
    // prepare the bounding box for its predicted collision
    m_clip[i].delta_check.min = m_transform[i].position + delta_pos - clip_box_size;
    m_clip[i].delta_check.max = m_transform[i].position + delta_pos + clip_box_size;
    
    // bounding box to detect collision exclusively on the x-axis
    m_clip[i].x_delta_check.min = m_transform[i].position + vec2_t(delta_pos.x, 0.0f) - clip_box_size;
    m_clip[i].x_delta_check.max = m_transform[i].position + vec2_t(delta_pos.x, 0.0f) + clip_box_size;
    
    // bounding box to detect collision exclusively on the y-axis
    m_clip[i].y_delta_check.min = m_transform[i].position + vec2_t(0.0f, delta_pos.y) - clip_box_size;
    m_clip[i].y_delta_check.max = m_transform[i].position + vec2_t(0.0f, delta_pos.y) + clip_box_size;
  }
}

void game_t::clip_map()
{
  for (int i = 0; i < m_num_entities; i++) {
    if (!m_clip[i].is_active())
      continue;
    
    if (aabb_map_check(m_clip[i].delta_check)) { // does the new position clip?
      if (!aabb_map_check(m_clip[i].x_delta_check)) // does it not clip if it's just the x-axis?
        m_clip[i].velocity_clip.y = 0.0f; // if so, only clip the y-velocity
      else if (!aabb_map_check(m_clip[i].y_delta_check)) // does it not clip if it's just the y-axis?
        m_clip[i].velocity_clip.x = 0.0f; // if so, only clip the x-velocity
      else
        m_clip[i].velocity_clip = vec2_t(); // the new position isn't free on the x or y axis
    }
  }
}

void game_t::clip_box()
{
  for (int i = 0; i < m_num_entities; i++) {
    if (!m_clip[i].is_active())
      continue;
    
    for (int j = 0; j < m_num_entities; j++) {
      if (!m_box[j].is_active())
        continue;
      
      // see game_t::clip_map()
      if (aabb_box_check(m_clip[i].delta_check, m_box[j].box)) {
        if (!aabb_box_check(m_clip[i].x_delta_check, m_box[j].box))
          m_clip[i].velocity_clip.y = 0.0f;
        else if (!aabb_box_check(m_clip[i].y_delta_check, m_box[j].box))
          m_clip[i].velocity_clip.x = 0.0f;
        else
          m_clip[i].velocity_clip = vec2_t();
      }
    }
  }
}
