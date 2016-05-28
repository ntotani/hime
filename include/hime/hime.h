#ifndef INCLUDE_HIME_HIME_H_
#define INCLUDE_HIME_HIME_H_

#include <string>
#include <unordered_map>
#include <vector>
#include "picojson/picojson.h"

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

const std::unordered_map<std::string, Planet> kPlanetStr = {
  {"sun", Planet::kSun},
  {"mon", Planet::kMoon},
  {"mar", Planet::kMars},
  {"mer", Planet::kMercury},
  {"jup", Planet::kJupiter},
  {"ven", Planet::kVenus},
  {"sat", Planet::kSaturn},
};

const std::vector<std::vector<int>> kPlanetRate = {
  {100, 100, 100, 100, 100, 100, 100},
  {100, 100, 100, 100, 100, 100, 100},
  {200, 100, 100,  50, 200, 100,  50},
  {200, 100, 200,  50,  50, 100,  50},
  {200, 100,  50, 200, 100, 100,  50},
  {100, 100, 100, 100, 100, 100, 100},
  { 50, 100, 100, 100, 100, 100, 100}
};

enum class PieceAction {
  kPhysical = 0,
  kMagical,
  kHeal,
};

const std::unordered_map<std::string, PieceAction> kPieceActionStr = {
  {"phys", PieceAction::kPhysical},
  {"magi", PieceAction::kMagical},
  {"heal", PieceAction::kHeal},
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
  Point():i(0), j(0) {}
  Point(int i, int j):i(i), j(j) {}
  bool operator==(const Point& rhs) const {
    return i == rhs.i && j == rhs.j;
  }
  Point operator+(const Point& rhs) const {
    return {i + rhs.i, j + rhs.j};
  }
  Point operator-(const Point& rhs) const {
    return {i - rhs.i, j - rhs.j};
  }
  picojson::value ToPicoValue() const {
    picojson::object obj;
    obj["i"] = picojson::value(static_cast<double>(i));
    obj["j"] = picojson::value(static_cast<double>(j));
    return std::move(picojson::value(obj));
  }
  static Point FromPicoValue(const picojson::value& val) {
    auto obj = val.get<picojson::object>();
    int i = static_cast<int>(obj["i"].get<double>());
    int j = static_cast<int>(obj["j"].get<double>());
    return Point(i, j);
  }
};

NS_HIME_END

#endif  // INCLUDE_HIME_HIME_H_

