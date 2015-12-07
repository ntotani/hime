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

class StorePiece {
 public:
  const MasterPiece &master_;
  explicit StorePiece(const MasterPiece &master):master_(master) {
  }
 private:
  DISALLOW_COPY_AND_ASSIGN(StorePiece);
};

NS_HIME_END

#endif  // INCLUDE_HIME_PIECE_H_

