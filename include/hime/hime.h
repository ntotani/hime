#ifndef INCLUDE_HIME_HIME_H_
#define INCLUDE_HIME_HIME_H_

#include <string>

#define NS_HIME_BEGIN namespace hime {
#define NS_HIME_END   }

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)

NS_HIME_BEGIN

enum Planet {
  kSun = 0,
  kMoon,
  kMars,
  kMercury,
  kJupiter,
  kVenus,
  kSaturn,
};

enum PieceAction {
  kPhysical = 0,
  kMagical,
  kHeal,
};

class Skill {
 public:
  const std::string id_;
  const std::string name_;
  const std::string desc_;
  const int rate_;
  Skill(const std::string &id, const std::string &name,
      const std::string &desc, int rate)
      :id_(id), name_(name), desc_(desc), rate_(rate) {
  }
 private:
  DISALLOW_COPY_AND_ASSIGN(Skill);
};

NS_HIME_END

#endif  // INCLUDE_HIME_HIME_H_

