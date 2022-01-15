/*

-- client.h --
"Middleman" for processing external input (net, keyboard, mouse)
and wrapping them to be processable by game_t. This is to be rewritten to
create a series of usercmd_t instead of directly accessing through client_t

*/

#ifndef CLIENT_H
#define CLIENT_H

class client_t {
private:
  int m_in_forward, m_in_back, m_in_right, m_in_left;
  int m_in_rot_right, m_in_rot_left;
  int m_mx, m_my;
  
public:
  client_t();
  
  void key_event(int key, int action);
  void mouse_move(float dx, float dy);
  
  int get_jmp() const;
  int get_forward() const;
  int get_right() const;
  int get_rot() const;
  float get_mouse_x() const;
  float get_mouse_y() const;
};

#endif
