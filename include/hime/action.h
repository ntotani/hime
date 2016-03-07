#ifndef INCLUDE_HIME_ACTION_H_
#define INCLUDE_HIME_ACTION_H_

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
    kEnd,
  };
  const Type type;
  explicit Action(Type type):type(type) {}
};

struct ActionMiss : public Action {
  const int actor_id, chip_idx;
  ActionMiss(int actor_id, int chip_idx)
      :Action(Type::kMiss), actor_id(actor_id), chip_idx(chip_idx) {}
};

struct ActionChip : public Action {
  const int actor_id, chip_idx;
  ActionChip(int actor_id, int chip_idx)
      :Action(Type::kChip), actor_id(actor_id), chip_idx(chip_idx) {}
};

struct ActionMove : public Action {
  const int actor_id;
  const Point from, to;
  ActionMove(int actor_id, Point from, Point to)
      :Action(Type::kMove), actor_id(actor_id), from(from), to(to) {}
};

struct ActionOb : public Action {
  const int actor_id;
  const Point pos;
  ActionOb(int actor_id, Point pos)
      :Action(Type::kOb), actor_id(actor_id), pos(pos) {}
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
};

NS_HIME_END

#endif  // INCLUDE_HIME_ACTION_H_

