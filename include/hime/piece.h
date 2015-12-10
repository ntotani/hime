#ifndef INCLUDE_HIME_PIECE_H_
#define INCLUDE_HIME_PIECE_H_

#include <string>
#include <utility>

#include "hime/hime.h"

NS_HIME_BEGIN

struct Parameter {
  const int power, defense, resist;
  Parameter(int power, int defense, int resist)
      :power(power), defense(defense), resist(resist) {}
};

class MasterPiece {
 public:
  const std::string id_;
  const std::string name_;
  const Planet planet_;
  const std::shared_ptr<const Skill> active_skill_;
  const std::shared_ptr<const Skill> passive_skill_;
  const Parameter param_;
  MasterPiece(const std::string &id, const std::string &name, Planet planet,
      std::shared_ptr<const Skill> active_skill,
      std::shared_ptr<const Skill> passive_skill,
      Parameter param)
      :id_(id),
      name_(name),
      planet_(planet),
      active_skill_(active_skill),
      passive_skill_(passive_skill),
      param_(param) {
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
      pump_(100, 100, 100) {
  }
 private:
  int team_;
  Point position_;
  int hp_;
  Parameter pump_;
  DISALLOW_COPY_AND_ASSIGN(SessionPiece);
};

NS_HIME_END

#endif  // INCLUDE_HIME_PIECE_H_

