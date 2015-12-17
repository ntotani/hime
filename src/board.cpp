#include "hime/board.h"


NS_HIME_BEGIN

using Tile::kNone;
using Tile::kNormal;
using Tile::kEvolution;

Board::Board(int board_id) {
  switch (board_id) {
    case 0:
      tiles_ = {
        {kNone, kNone, kNormal, kNone, kNone},
        {kNone, kNormal, kNone, kNormal, kNone},
        {kNormal, kNone, kNormal, kNone, kNormal},
        {kNone, kNormal, kNone, kNormal, kNone},
        {kNormal, kNone, kNormal, kNone, kNormal},
        {kNone, kNormal, kNone, kNormal, kNone},
        {kNormal, kNone, kNormal, kNone, kNormal},
        {kNone, kNormal, kNone, kNormal, kNone},
        {kNone, kNone, kNormal, kNone, kNone}
      };
      break;
    default:
      tiles_ = {
        {kNone, kNone, kNone, kNormal, kNone, kNone, kNone},
        {kNone, kNone, kNormal, kNone, kNormal, kNone, kNone},
        {kNone, kNormal, kNone, kNormal, kNone, kEvolution, kNone},
        {kNormal, kNone, kNormal, kNone, kNormal, kNone, kNormal},
        {kNone, kNormal, kNone, kNormal, kNone, kNormal, kNone},
        {kNormal, kNone, kNormal, kNone, kNormal, kNone, kNormal},
        {kNone, kNormal, kNone, kNormal, kNone, kNormal, kNone},
        {kNormal, kNone, kNormal, kNone, kNormal, kNone, kNormal},
        {kNone, kNormal, kNone, kNormal, kNone, kNormal, kNone},
        {kNormal, kNone, kNormal, kNone, kNormal, kNone, kNormal},
        {kNone, kNormal, kNone, kNormal, kNone, kNormal, kNone},
        {kNone, kNone, kNormal, kNone, kNormal, kNone, kNone},
        {kNone, kNone, kNone, kNormal, kNone, kNone, kNone}
      };
  }
}

bool Board::IsOutOfBounce(Point position) const {
  return
    position.i < 0 ||
    static_cast<size_t>(position.i) >= tiles_.size() ||
    position.j < 0 ||
    static_cast<size_t>(position.j) >= tiles_[position.i].size();
}

NS_HIME_END

