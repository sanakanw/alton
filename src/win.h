#ifndef WIN_H
#define WIN_H

#include "client.h"

void	win_api_init();
float	win_get_time();

class win_t {
private:
  bool m_quit;
  
public:
  win_t(int width, int height);
  
  void poll(client_t &client);
  void quit();
  void swap();
  
  bool is_quit();
};

#endif
