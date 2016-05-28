#ifndef INCLUDE_HIME_SKILL_H_
#define INCLUDE_HIME_SKILL_H_

#include <string>
#include <stack>
#include <memory>

#include "hime/hime.h"

NS_HIME_BEGIN

class Session;
struct Action;

class SkillEffect {
  virtual bool PreAction(Session* session,
      const std::unique_ptr<Action>& action,
      std::stack<std::unique_ptr<Action>>* resolve) const { return false; }
};

class Skill {
 public:
  Skill(const std::string &id, const std::string &name, const std::string &desc,
      const std::shared_ptr<SkillEffect> effect)
      :id_(id), name_(name), desc_(desc), effect_(effect) {
  }
  inline const std::string& id() const { return id_; }
  inline const std::string& name() const { return name_; }
  inline const std::string& desc() const { return desc_; }
  inline const std::shared_ptr<SkillEffect>& effect() const { return effect_; }

 private:
  const std::string id_;
  const std::string name_;
  const std::string desc_;
  const std::shared_ptr<SkillEffect> effect_;
  DISALLOW_COPY_AND_ASSIGN(Skill);
};

class SkillRevenge : public SkillEffect {
  bool PreAction(Session* session,
      const std::unique_ptr<Action>& action,
      std::stack<std::unique_ptr<Action>>* resolve) const override;
};

NS_HIME_END

#endif  // INCLUDE_HIME_SKILL_H_

