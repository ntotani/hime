#ifndef INCLUDE_HIME_PIECE_H_
#define INCLUDE_HIME_PIECE_H_

#include <string>
#include <utility>

#include "hime/hime.h"

NS_HIME_BEGIN

class MasterPiece {
 public:
  const std::string id_;
  const std::string name_;
  const Planet planet_;
  const std::shared_ptr<const Skill> active_skill_;
  const std::shared_ptr<const Skill> passive_skill_;
  const int power_;
  const int defense_;
  const int resist_;
  MasterPiece(const std::string &id, const std::string &name, Planet planet,
      std::shared_ptr<const Skill> active_skill,
      std::shared_ptr<const Skill> passive_skill,
      int power, int defense, int resist)
      :id_(id),
      name_(name),
      planet_(planet),
      active_skill_(active_skill),
      passive_skill_(passive_skill),
      power_(power),
      defense_(defense),
      resist_(resist) {
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(MasterPiece);
};

class OwnedPiece {
 public:
  const std::shared_ptr<const MasterPiece> master_;
  const std::string id_;
  OwnedPiece(std::shared_ptr<const MasterPiece> master, const std::string &id)
      :master_(master), id_(id) {
  }
 private:
  DISALLOW_COPY_AND_ASSIGN(OwnedPiece);
};

class SessionPiece {
 public:
  const std::shared_ptr<const OwnedPiece> owned_;
  const int id_;
  SessionPiece(std::shared_ptr<const OwnedPiece> owned,
      int id, int team, Point position)
      :owned_(owned), id_(id), team_(team), position_(position), hp_(100),
      pump_power_(1.0), pump_defense_(1.0), pump_resist_(1.0) {
  }
 private:
  int team_;
  Point position_;
  int hp_;
  float pump_power_;
  float pump_defense_;
  float pump_resist_;
  DISALLOW_COPY_AND_ASSIGN(SessionPiece);
};

NS_HIME_END

#endif  // INCLUDE_HIME_PIECE_H_

