#include "game.h"

float player_accelerate(const vec2_t &prev_velocity, const vec2_t &wish_dir, float accel, float wish_speed)
{
  float current_speed = prev_velocity.dot(wish_dir);
  float add_speed = wish_speed - current_speed;
  
  if (add_speed <= 0)
    return 0;
  
  float accel_speed = accel * wish_speed;
  
  if (accel_speed > add_speed)
    accel_speed = add_speed;
  
  return accel_speed;
}

void game_t::camera_rotate()
{
  const float CAMERA_ROT_SPEED = 2.0f;
  m_camera.view_angle += m_client.get_rot() * CAMERA_ROT_SPEED * -m_delta_time;
}

void game_t::animate_player()
{
  const float WALK_FRAME_TIME = 0.5f;
  
  float face_dir = constrain_angle(m_transform[m_player_entity].rotation - m_camera.view_angle);
  
  if (face_dir > 1.0f / 6.0f * M_PI && face_dir <= 5.0f / 6.0f * M_PI) // FACING FORWARD
    m_sprite[m_player_entity].state = 0;
  else if (face_dir > 5.0f / 6.0f * M_PI && face_dir <= 7.0f / 6.0f * M_PI) // FACING LEFT
    m_sprite[m_player_entity].state = 1;
  else if (face_dir > 7.0f / 6.0f * M_PI && face_dir <= 11.0f / 6.0f * M_PI) // FACING DOWN
    m_sprite[m_player_entity].state = 2;
  else // FACING RIGHT (NOTE: [0, 30], [330, 360]) 
    m_sprite[m_player_entity].state = 3;
  
  float t = m_time - m_anim[m_player_entity].prev_frame;
  
  if (m_motion[m_player_entity].velocity.length() > 1.0f) {
    if (t > WALK_FRAME_TIME) {
      float over_time = t - WALK_FRAME_TIME;
      m_anim[m_player_entity].prev_frame = m_time + over_time;
      t = over_time;
    }
    
    if (t > WALK_FRAME_TIME / 2.0f)
      m_sprite[m_player_entity].frame = 2;
    else if (t > 0.0f)
      m_sprite[m_player_entity].frame = 1;
  } else {
    m_sprite[m_player_entity].frame = 0;
    m_anim[m_player_entity].prev_frame = m_time;
  }
}

void game_t::player_move()
{
  const float PLAYER_MOVE_ACCEL = 8.0f;
  const float PLAYER_MOVE_SPEED = 8.0f;
  
  if (m_client.get_right() || m_client.get_forward()) {
    vec2_t move_dir = vec2_t(m_client.get_right(), m_client.get_forward()).normalize();
    vec2_t wish_dir = move_dir.rotate(m_camera.view_angle);
    
    float accel = player_accelerate(
      m_motion[m_player_entity].velocity,
      wish_dir,
      PLAYER_MOVE_ACCEL * m_delta_time,
      PLAYER_MOVE_SPEED);
    
    m_motion[m_player_entity].velocity = wish_dir * PLAYER_MOVE_SPEED;
    m_transform[m_player_entity].rotation = wish_dir.to_angle();
  } else {
    m_motion[m_player_entity].velocity = vec2_t();
  }
}

void game_t::lock_camera_on_player()
{
  if (!m_transform[m_player_entity].is_active())
    return;
  
  m_camera.view_origin = m_transform[m_player_entity].position;
}
