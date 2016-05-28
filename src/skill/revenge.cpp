#include <stack>
#include <memory>

#include "hime/skill.h"

using std::stack;
using std::unique_ptr;

NS_HIME_BEGIN

bool SkillRevenge::PreAction(Session* session, const unique_ptr<Action>& action,
    stack<unique_ptr<Action>>* resolve) const {
  return false;
}

NS_HIME_END

