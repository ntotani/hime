#include "hime/session.h"

NS_HIME_BEGIN

Session::Session(int player_num, std::function<int()> random)
    :player_num_(player_num), random_(random), state_(0) {
}

void Session::apply() {
  state_ = random_();
}

NS_HIME_END

