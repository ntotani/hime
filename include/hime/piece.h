#ifndef INCLUDE_HIME_PIECE_H_
#define INCLUDE_HIME_PIECE_H_

#include <string>

#include "hime/hime.h"

NS_HIME_BEGIN

class MasterPiece {
 public:
  const int id_;
  const std::string name_;
  const int power_;
  const int defense_;
  const int resist_;
  const int evo_;
  MasterPiece(int id, const std::string &name, int power, int defense,
    int resist, int evo)
    :id_(id),
    name_(name),
    power_(power),
    defense_(defense),
    resist_(resist),
    evo_(evo) {
  }
 private:
  DISALLOW_COPY_AND_ASSIGN(MasterPiece);
};

NS_HIME_END

#endif  // INCLUDE_HIME_PIECE_H_

