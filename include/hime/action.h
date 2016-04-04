#ifndef INCLUDE_HIME_ACTION_H_
#define INCLUDE_HIME_ACTION_H_

#include <string>
#include <sstream>
#include "hime/hime.h"

NS_HIME_BEGIN

struct Action {
  enum class Type {
    kMiss = 0,
    kChip,
    kMove,
    kSwap,
    kOb,
    kAttack,
    kHeal,
    kPskill,
    kEvo,
    kDrop,
  };
  const Type type;
  explicit Action(Type type):type(type) {}
  virtual std::string ToString() const = 0;
};

/*
struct ActionMiss : public Action {
  const int actor_id, chip_idx;
  ActionMiss(int actor_id, int chip_idx)
      :Action(Type::kMiss), actor_id(actor_id), chip_idx(chip_idx) {}
};
*/

struct ActionChip : public Action {
  const int actor_id, chip_idx;
  ActionChip(int actor_id, int chip_idx)
      :Action(Type::kChip), actor_id(actor_id), chip_idx(chip_idx) {}
  std::string ToString() const override {
    std::ostringstream s;
    s << "{\"type\":\"chip\",\"actor_id\":" << actor_id;
    s << ",\"chip_idx\":" << chip_idx << "}";
    return s.str();
  }
};

struct ActionMove : public Action {
  const int actor_id;
  const Point from, to;
  ActionMove(int actor_id, Point from, Point to)
      :Action(Type::kMove), actor_id(actor_id), from(from), to(to) {}
  std::string ToString() const override {
    std::ostringstream s;
    s << "{\"type\":\"move\",\"actor_id\":" << actor_id;
    s << ",\"from\":" << from.ToString() << ",\"to\":" << to.ToString() << "}";
    return s.str();
  }
};

struct ActionOb : public Action {
  const int actor_id;
  const Point pos;
  ActionOb(int actor_id, Point pos)
      :Action(Type::kOb), actor_id(actor_id), pos(pos) {}
  std::string ToString() const override {
    std::ostringstream s;
    s << "{\"type\":\"ob\",\"actor_id\":" << actor_id;
    s << ",\"pos\":" << pos.ToString() << "}";
    return s.str();
  }
};

struct ActionAttack : public Action {
  const int actor_id;
  const int target_id;
  const Point from, to;
  const int hp, dmg;
  ActionAttack(int actor_id, int target_id,
      Point from, Point to, int hp, int dmg)
      :Action(Type::kAttack), actor_id(actor_id), target_id(target_id),
      from(from), to(to), hp(hp), dmg(dmg) {}
  std::string ToString() const override {
    std::ostringstream s;
    s << "{\"type\":\"attack\",\"actor_id\":" << actor_id;
    s << ",\"target_id\":" << target_id;
    s << ",\"from\":" << from.ToString() << ",\"to\":" << to.ToString();
    s << ",\"hp\":" << hp << ",\"dmg\":" << dmg << "}";
    return s.str();
  }
};

struct ActionHeal : public Action {
  const int actor_id;
  const int target_id;
  const Point from, to;
  const int hp, gain;
  ActionHeal(int actor_id, int target_id,
      Point from, Point to, int hp, int gain)
      :Action(Type::kHeal), actor_id(actor_id), target_id(target_id),
      from(from), to(to), hp(hp), gain(gain) {}
  std::string ToString() const override {
    std::ostringstream s;
    s << "{\"type\":\"heal\",\"actor_id\":" << actor_id;
    s << ",\"target_id\":" << target_id;
    s << ",\"from\":" << from.ToString() << ",\"to\":" << to.ToString();
    s << ",\"hp\":" << hp << ",\"gain\":" << gain << "}";
    return s.str();
  }
};

struct ActionDrop : public Action {
  const int team_id;
  explicit ActionDrop(int team_id)
      :Action(Type::kDrop), team_id(team_id) {}
  std::string ToString() const override {
    std::ostringstream s;
    s << "{\"type\":\"drop\",\"team_id\":" << team_id << "}";
    return s.str();
  }
};

NS_HIME_END

#endif  // INCLUDE_HIME_ACTION_H_

