#include "game.h"

void game_t::update_sprite()
{
  for (int i = 0; i < m_num_entities; i++) {
    if (!m_sprite[i].is_active() || !m_transform[i].is_active())
      continue;
    
    m_sprite[i].position = m_transform[i].position;
    m_sprite[i].rotation = m_transform[i].rotation;
  }
}
