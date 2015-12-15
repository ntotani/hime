#ifndef INCLUDE_HIME_MASTER_H_
#define INCLUDE_HIME_MASTER_H_

#include <string>
#include <unordered_map>

#include "hime/piece.h"

NS_HIME_BEGIN

class Master {
 public:
  Master() {}
  void LoadPiece(std::string yaml);
  void LoadSkill(std::string yaml);
  std::shared_ptr<const MasterPiece> piece(std::string id) const {
    return piece_.at(id); }
  std::shared_ptr<const Skill> skill(std::string id) const {
    return skill_.at(id); }
 private:
  std::unordered_map<std::string, std::shared_ptr<const MasterPiece>> piece_;
  std::unordered_map<std::string, std::shared_ptr<const Skill>> skill_;
  DISALLOW_COPY_AND_ASSIGN(Master);
};

NS_HIME_END

#endif  // INCLUDE_HIME_MASTER_H_

