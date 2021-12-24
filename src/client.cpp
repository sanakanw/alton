#include "client.h"

#include <iostream>

client_t::client_t()
{
  m_in_left = 0;
  m_in_right = 0;
  m_in_forward = 0;
  m_in_back = 0;
  m_mx = 0;
  m_my = 0;
}

void client_t::key_event(int key, int action)
{
  switch (key) {
  case 'w':
    m_in_forward = action;
    break;
  case 's':
    m_in_back = action;
    break;
  case 'd':
    m_in_right = action;
    break;
  case 'a':
    m_in_left = action;
    break;
  case 'q':
    m_in_rot_left = action;
    break;
  case 'e':
    m_in_rot_right = action;
    break;
  }
}

void client_t::mouse_move(float dx, float dy)
{
  m_mx += dx;
  m_my += dy;
}

int client_t::get_forward() const
{
  return m_in_forward - m_in_back;
}

int client_t::get_right() const
{
  return m_in_right - m_in_left;
}

int client_t::get_rot() const
{
  return m_in_rot_right - m_in_rot_left;
}

float client_t::get_yaw() const
{
  return m_mx;
}

float client_t::get_pitch() const
{
  return m_my;
}
