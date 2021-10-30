#ifndef GAMEDATA_HPP_
#define GAMEDATA_HPP_

#include <bitset>

enum class Input { Down, Up, Fire, Jump };
enum class State { Playing, GameOver, NotRunning };

struct GameData {
  State m_state{State::NotRunning};
  std::bitset<5> m_input;  // [fire, up, down, left, right]
};

#endif