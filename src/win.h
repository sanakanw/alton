#ifndef WIN_H
#define WIN_H

#include "client.h"

class win_t {
private:
  bool m_quit;
  
public:
  static void init();
  static float get_time();
  
  win_t(int width, int height);
  
  void poll(client_t &client);
  void quit();
  void swap();
  
  bool is_quit();
};

#endif
