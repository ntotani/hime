#include "hime/action.h"

#include <string>
#include <vector>

NS_HIME_BEGIN

using std::string;
using std::move;
using std::unique_ptr;
using std::make_unique;
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
  obj["dmg"]       = value(static_cast<double>(dmg));
  return move(value(obj));
}

value ActionHeal::ToPicoValue() const {
  object obj;
  obj["type"]      = value("heal");
  obj["actor_id"]  = value(static_cast<double>(actor_id));
  obj["target_id"] = value(static_cast<double>(target_id));
  obj["gain"]      = value(static_cast<double>(gain));
  return move(value(obj));
}

value ActionDrop::ToPicoValue() const {
  object obj;
  obj["type"]    = value("drop");
  obj["team_id"] = value(static_cast<double>(team_id));
  return move(value(obj));
}

unique_ptr<Action> Action::FromPicoValue(const value& val) {
  auto obj = val.get<object>();
  auto type = obj["type"].get<string>();
  if (type == "chip") {
    int actor_id = static_cast<int>(obj["actor_id"].get<double>());
    int chip_idx = static_cast<int>(obj["chip_idx"].get<double>());
    return make_unique<ActionChip>(actor_id, chip_idx);
  } else if (type == "move") {
    int actor_id = static_cast<int>(obj["actor_id"].get<double>());
    auto to = Point::FromPicoValue(obj["to"]);
    return make_unique<ActionMove>(actor_id, to);
  } else if (type == "ob") {
    int actor_id = static_cast<int>(obj["actor_id"].get<double>());
    auto pos = Point::FromPicoValue(obj["pos"]);
    return make_unique<ActionOb>(actor_id, pos);
  } else if (type == "attack") {
    int actor_id = static_cast<int>(obj["actor_id"].get<double>());
    int target_id = static_cast<int>(obj["target_id"].get<double>());
    int dmg = static_cast<int>(obj["dmg"].get<double>());
    return make_unique<ActionAttack>(actor_id, target_id, dmg);
  } else if (type == "heal") {
    int actor_id = static_cast<int>(obj["actor_id"].get<double>());
    int target_id = static_cast<int>(obj["target_id"].get<double>());
    int gain = static_cast<int>(obj["gain"].get<double>());
    return make_unique<ActionHeal>(actor_id, target_id, gain);
  } else if (type == "drop") {
    int team_id = static_cast<int>(obj["team_id"].get<double>());
    return make_unique<ActionDrop>(team_id);
  }
}

NS_HIME_END

