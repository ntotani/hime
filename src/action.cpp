#include "hime/action.h"

#include <vector>

NS_HIME_BEGIN

using std::move;
using picojson::value;
using picojson::object;

value ActionChip::ToPicoValue() const {
  object obj;
  obj["type"]     = value("chip");
  obj["actor_id"] = value(static_cast<double>(actor_id));
  obj["chip_idx"] = value(static_cast<double>(chip_idx));
  return move(value(obj));
}

value ActionMove::ToPicoValue() const {
  object obj;
  obj["type"]     = value("move");
  obj["actor_id"] = value(static_cast<double>(actor_id));
  obj["from"]     = from.ToPicoValue();
  obj["to"]       = to.ToPicoValue();
  return move(value(obj));
}

value ActionOb::ToPicoValue() const {
  object obj;
  obj["type"]     = value("ob");
  obj["actor_id"] = value(static_cast<double>(actor_id));
  obj["pos"]      = pos.ToPicoValue();
  return move(value(obj));
}

value ActionAttack::ToPicoValue() const {
  object obj;
  obj["type"]      = value("attack");
  obj["actor_id"]  = value(static_cast<double>(actor_id));
  obj["target_id"] = value(static_cast<double>(target_id));
  obj["from"]      = from.ToPicoValue();
  obj["to"]        = to.ToPicoValue();
  obj["hp"]        = value(static_cast<double>(hp));
  obj["dmg"]       = value(static_cast<double>(dmg));
  return move(value(obj));
}

value ActionHeal::ToPicoValue() const {
  object obj;
  obj["type"]      = value("heal");
  obj["actor_id"]  = value(static_cast<double>(actor_id));
  obj["target_id"] = value(static_cast<double>(target_id));
  obj["from"]      = from.ToPicoValue();
  obj["to"]        = to.ToPicoValue();
  obj["hp"]        = value(static_cast<double>(hp));
  obj["gain"]      = value(static_cast<double>(gain));
  return move(value(obj));
}

value ActionDrop::ToPicoValue() const {
  object obj;
  obj["type"]      = value("drop");
  obj["team_id"]  = value(static_cast<double>(team_id));
  return move(value(obj));
}

NS_HIME_END

