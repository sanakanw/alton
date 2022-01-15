#include "game.h"

/*
  All systems related to player movement and animation
*/

// Rotate the camera
void game_t::camera_rotate()
{
  const float CAMERA_ROT_SPEED = 2.0f;
  m_camera.view_angle += m_client.get_rot() * CAMERA_ROT_SPEED * -m_delta_time;
}

// Animates the player
void game_t::animate_player()
{
  // total time to cycle through a single walk animation
  const float WALK_FRAME_TIME = 0.5f;
  
  // angle player is facing relative to the camera (based off the last direciton the player moved in)
  float face_dir = constrain_angle(m_transform[m_player_entity].rotation - m_camera.view_angle);
  
  // the facing direction determines the player sprite state
  // NOTE: uses radians
  if (face_dir > 1.0f / 6.0f * M_PI && face_dir <= 5.0f / 6.0f * M_PI) // (30, 150)
    m_sprite[m_player_entity].state = 0; // FACING FORARD
  else if (face_dir > 5.0f / 6.0f * M_PI && face_dir <= 7.0f / 6.0f * M_PI) // (150, 210)
    m_sprite[m_player_entity].state = 1; // FACING LEFT
  else if (face_dir > 7.0f / 6.0f * M_PI && face_dir <= 11.0f / 6.0f * M_PI) // (210, 330)
    m_sprite[m_player_entity].state = 2; // FACING DOWN
  else // [0, 30], [330, 360]
    m_sprite[m_player_entity].state = 3; // FACING RIGHT
  
  // time since last frame;
  float t = m_time - m_anim[m_player_entity].prev_frame;
  
  // check if the player is moving
  if (m_motion[m_player_entity].velocity.length() > 1.0f) {
    
    // if the time exceeds a full walk frame, reset it
    if (t > WALK_FRAME_TIME) {
      float over_time = t - WALK_FRAME_TIME;
      m_anim[m_player_entity].prev_frame = m_time + over_time;
      t = over_time;
    }
    
    // written like a piecewise function based on 't' to determine the frame
    // conditions stacked latest to earliest (top to bottom) to reduce number
    // of conditions in the if statement
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
    // derive the moving direction from the inputs and camera rotation
    vec2_t move_dir = vec2_t(m_client.get_right(), m_client.get_forward()).normalize();
    vec2_t wish_dir = move_dir.rotate(m_camera.view_angle);
    
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
