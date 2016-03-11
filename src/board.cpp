#include "hime/board.h"

#include <vector>

NS_HIME_BEGIN

using std::vector;
using std::unique_ptr;
using std::make_unique;
using Tile::Type::kNone;
using Tile::Type::kNormal;
using Tile::Type::kEvolution;

Board::Board(int board_id) {
  vector<vector<Tile::Type>> types;
  switch (board_id) {
    case 0:
      types = {
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
      types = {
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
  for (auto &row : types) {
    vector<unique_ptr<Tile>> line;
    for (auto e : row) {
      line.push_back(make_unique<Tile>(e));
    }
    tiles_.push_back(move(line));
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

