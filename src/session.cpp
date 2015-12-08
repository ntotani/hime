#include "hime/session.h"

NS_HIME_BEGIN

void Session::applyAction() {
  state_ = context_.random();
}

NS_HIME_END

