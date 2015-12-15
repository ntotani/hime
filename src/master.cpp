#include "hime/master.h"

#include <string>

using std::string;
using std::make_shared;

NS_HIME_BEGIN

void Master::LoadPiece(string yaml) {
  piece_["1"] = make_shared<const MasterPiece>("1", "姫",
      Planet::kSun, PieceAction::kHeal, skill_["a1"], skill_["p1"],
      hime::Parameter(60, 50, 80));
  piece_["3"] = make_shared<const MasterPiece>("3", "浪人",
      Planet::kMars, PieceAction::kPhysical, skill_["a3"], skill_["p3"],
      hime::Parameter(80, 80, 60));
}

void Master::LoadSkill(string yaml) {
  skill_["a1"] = make_shared<const Skill>(
      "a1", "全体回復", "味方全員を@回復する", 30);
  skill_["a3"] = make_shared<const Skill>(
      "a3", "突撃", "攻撃力2倍で2マス前進", 0);
  skill_["p1"] = make_shared<const Skill>(
      "p1", "癒やし", "周りの駒が毎ターン@ずつ回復する", 30);
  skill_["p3"] = make_shared<const Skill>(
      "p3", "一矢", "この駒を倒した相手に攻撃する", 0);
}

NS_HIME_END

