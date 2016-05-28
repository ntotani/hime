#ifndef INCLUDE_HIME_PIECE_H_
#define INCLUDE_HIME_PIECE_H_

#include <string>
#include <vector>
#include <utility>
#include <memory>

#include "hime/hime.h"

NS_HIME_BEGIN

class Skill;

struct Parameter {
  const int power, defense, resist;
  explicit Parameter(int value)
      :power(value), defense(value), resist(value) {}
  Parameter(int power, int defense, int resist)
      :power(power), defense(defense), resist(resist) {}
  Parameter operator*(const Parameter& rhs) const {
    return {power * rhs.power, defense * rhs.defense, resist * rhs.resist};
  }
  Parameter operator/(const int& rhs) const {
    return {power / rhs, defense / rhs, resist / rhs};
  }
};

class MasterPiece {
 public:
  MasterPiece(const std::string &id, const std::string &name,
      Planet planet, PieceAction action,
      std::shared_ptr<const Skill> passive_skill,
      Parameter param, const std::vector<Point> &range)
      :id_(id),
      name_(name),
      planet_(planet),
      action_(action),
      passive_skill_(passive_skill),
      param_(param),
      range_(range) {
  }
  inline const std::string& id() const { return id_; }
  inline const std::string& name() const { return name_; }
  inline Planet planet() const { return planet_; }
  inline PieceAction action() const { return action_; }
  inline std::shared_ptr<const Skill> passive_skill() const {
      return passive_skill_; }
  inline Parameter param() const { return param_; }
  inline const std::vector<Point>& range() const { return range_; }

 private:
  const std::string id_;
  const std::string name_;
  const Planet planet_;
  const PieceAction action_;
  const std::shared_ptr<const Skill> passive_skill_;
  const Parameter param_;
  const std::vector<Point> range_;
  DISALLOW_COPY_AND_ASSIGN(MasterPiece);
};

class OwnedPiece {
 public:
  OwnedPiece(std::shared_ptr<const MasterPiece> master, const std::string &id)
      :master_(master), id_(id) {
  }
  inline std::shared_ptr<const MasterPiece> master() const { return master_; }
  inline const std::string& id() const { return id_; }

 private:
  const std::shared_ptr<const MasterPiece> master_;
  const std::string id_;
  DISALLOW_COPY_AND_ASSIGN(OwnedPiece);
};

struct SessionPieceState {
  const Point position;
  const int hp;
  const Parameter pump;
};

class Session;

class SessionPiece {
  friend Session;

 public:
  SessionPiece(std::shared_ptr<const OwnedPiece> owned,
      int id, int team, Point position)
      :owned_(owned), id_(id), team_(team), position_(position), hp_(100),
      pump_(100) {
  }
  SessionPiece(std::shared_ptr<const OwnedPiece> owned,
      int id, int team, SessionPieceState state)
      :owned_(owned), id_(id), team_(team),
      position_(state.position), hp_(state.hp), pump_(state.pump) {
  }
  inline std::shared_ptr<const OwnedPiece> owned() const { return owned_; }
  inline int id() const { return id_; }
  inline int team() const { return team_; }
  inline Point position() const { return position_; }
  inline int hp() const { return hp_; }
  inline Parameter pump() const { return pump_; }
  inline Parameter param() const {
    return owned_->master()->param() * pump_ / 100;
  }
  inline PieceAction action() const { return owned_->master()->action(); }
  inline Planet planet() const { return owned_->master()->planet(); }
  inline const std::vector<Point>& range() const {
    return owned_->master()->range();
  }

 private:
  const std::shared_ptr<const OwnedPiece> owned_;
  const int id_;
  int team_;
  Point position_;
  int hp_;
  Parameter pump_;
  DISALLOW_COPY_AND_ASSIGN(SessionPiece);
};

NS_HIME_END

#endif  // INCLUDE_HIME_PIECE_H_

