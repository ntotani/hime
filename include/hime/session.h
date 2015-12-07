#ifndef INCLUDE_HIME_SESSION_H_
#define INCLUDE_HIME_SESSION_H_

#include <functional>

#include "hime/hime.h"

NS_HIME_BEGIN

class Session {
 public:
  const int player_num_;
  const std::function<int()> random_;
  Session(int player_num, std::function<int()> random);
  void apply();
  inline int getState() const { return state_; }
 private:
  int state_;
  DISALLOW_COPY_AND_ASSIGN(Session);
};

NS_HIME_END

#endif  // INCLUDE_HIME_SESSION_H_

