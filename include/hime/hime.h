#ifndef INCLUDE_HIME_HIME_H_
#define INCLUDE_HIME_HIME_H_

#include <string>

#define NS_HIME_BEGIN namespace hime {
#define NS_HIME_END   }

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)

NS_HIME_BEGIN

enum class Planet {
  kSun = 0,
  kMoon,
  kMars,
  kMercury,
  kJupiter,
  kVenus,
  kSaturn,
};

enum class PieceAction {
  kPhysical = 0,
  kMagical,
  kHeal,
};

class Skill {
 public:
  Skill(const std::string &id, const std::string &name,
      const std::string &desc, int rate)
      :id_(id), name_(name), desc_(desc), rate_(rate) {
  }
  inline const std::string& id() const { return id_; }
  inline const std::string& name() const { return name_; }
  inline const std::string& desc() const { return desc_; }
  inline int rate() const { return rate_; }

 private:
  const std::string id_;
  const std::string name_;
  const std::string desc_;
  const int rate_;
  DISALLOW_COPY_AND_ASSIGN(Skill);
};

enum class Tile {
  kNone = 0,
  kNormal,
  kEvolution,
};

enum class Card {
  kFront = 0,
  kFrontR,
  kFrontL,
  kBackR,
  kBackL,
  kBack,
  kFrontFront,
  kFrontRFrontR,
  kFrontLFrontL,
  kFrontFrontR,
  kFrontFrontL,
  kSkill,
};

struct Point {
  int i, j;
  bool operator==(const Point &rhs) const {
    return i == rhs.i && j == rhs.j;
  }
  Point operator+(const Point &rhs) const {
    return {i + rhs.i, j + rhs.j};
  }
};

NS_HIME_END

#endif  // INCLUDE_HIME_HIME_H_

