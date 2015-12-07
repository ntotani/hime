#ifndef INCLUDE_HIME_PIECE_H_
#define INCLUDE_HIME_PIECE_H_

#include <string>

#include "hime/hime.h"

NS_HIME_BEGIN

class MasterPiece {
 public:
  const std::string id_;
  const std::string name_;
  const Planet planet_;
  const Skill &active_skill_;
  const Skill &passive_skill_;
  const int power_;
  const int defense_;
  const int resist_;
  MasterPiece(const std::string &id, const std::string &name, Planet planet,
      const Skill &active_skill, const Skill &passive_skill,
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
  const MasterPiece &master_;
  explicit OwnedPiece(const MasterPiece &master):master_(master) {
  }
 private:
  DISALLOW_COPY_AND_ASSIGN(OwnedPiece);
};

class SessionPiece {
 public:
  const OwnedPiece &owned_;
  int id_;
  SessionPiece(const OwnedPiece &owned, int id, int team, int i, int j)
      :owned_(owned), id_(id), team_(team), i_(i), j_(j), hp_(100),
      pump_power_(1.0), pump_defense_(1.0), pump_resist_(1.0) {
  }
 private:
  int team_;
  int i_;
  int j_;
  int hp_;
  float pump_power_;
  float pump_defense_;
  float pump_resist_;
  DISALLOW_COPY_AND_ASSIGN(SessionPiece);
};

NS_HIME_END

#endif  // INCLUDE_HIME_PIECE_H_

