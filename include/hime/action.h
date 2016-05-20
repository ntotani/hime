#ifndef INCLUDE_HIME_ACTION_H_
#define INCLUDE_HIME_ACTION_H_

#include <memory>
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
  virtual picojson::value ToPicoValue() const = 0;
  static std::unique_ptr<Action> FromPicoValue(const picojson::value& val);
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
  picojson::value ToPicoValue() const override;
};

struct ActionMove : public Action {
  const int actor_id;
  const Point to;
  ActionMove(int actor_id, Point to)
      :Action(Type::kMove), actor_id(actor_id), to(to) {}
  picojson::value ToPicoValue() const override;
};

struct ActionOb : public Action {
  const int actor_id;
  const Point pos;
  ActionOb(int actor_id, Point pos)
      :Action(Type::kOb), actor_id(actor_id), pos(pos) {}
  picojson::value ToPicoValue() const override;
};

struct ActionAttack : public Action {
  const int actor_id;
  const int target_id;
  const int dmg;
  ActionAttack(int actor_id, int target_id, int dmg)
      :Action(Type::kAttack), actor_id(actor_id),
      target_id(target_id), dmg(dmg) {}
  picojson::value ToPicoValue() const override;
};

struct ActionHeal : public Action {
  const int actor_id;
  const int target_id;
  const int gain;
  ActionHeal(int actor_id, int target_id, int gain)
      :Action(Type::kHeal), actor_id(actor_id),
      target_id(target_id), gain(gain) {}
  picojson::value ToPicoValue() const override;
};

struct ActionDrop : public Action {
  const int team_id;
  explicit ActionDrop(int team_id)
      :Action(Type::kDrop), team_id(team_id) {}
  picojson::value ToPicoValue() const override;
};

NS_HIME_END

#endif  // INCLUDE_HIME_ACTION_H_

