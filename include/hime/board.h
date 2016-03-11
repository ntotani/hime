#ifndef INCLUDE_HIME_BOARD_H_
#define INCLUDE_HIME_BOARD_H_

#include <vector>

#include "hime/hime.h"

NS_HIME_BEGIN

struct Tile {
  enum class Type {
    kNone = 0,
    kNormal,
    kEvolution,
  };
  const Type type;
  explicit Tile(Type type):type(type) {}
};

class Board {
 public:
  explicit Board(int board_id);
  bool IsOutOfBounce(Point position) const;
  inline const std::vector<std::vector<std::unique_ptr<Tile>>>& tiles() const {
    return tiles_;
  }
 private:
  std::vector<std::vector<std::unique_ptr<Tile>>> tiles_;
  DISALLOW_COPY_AND_ASSIGN(Board);
};

NS_HIME_END

#endif  // INCLUDE_HIME_BOARD_H_

