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

// input:
// key_event()
// mouse_move()

// as of now, only letters and numbers are mapped to respective ascii codes
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

// output:
// get_mouse_x()
// get_mouse_y()
// get_forward()
// get_right()
// get_rot()

float client_t::get_mouse_x() const
{
  return m_mx;
}

float client_t::get_mouse_y() const
{
  return m_my;
}

// for get_forward(), get_right() and get_rot() the booleans are "merged"
// this allows an axis to be represented in a single value
// and behaviour such as if the keys for the respective axis are both pressed
// the value will be 0 and as such not move
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
