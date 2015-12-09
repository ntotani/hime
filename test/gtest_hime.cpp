#include <vector>

#include "gtest/gtest.h"

#include "hime/piece.h"
#include "hime/session.h"

using std::vector;
using hime::MasterPiece;
using hime::OwnedPiece;
using hime::SessionPiece;
using hime::Skill;
using hime::Session;
using hime::SessionContext;
using hime::SessionContextImpl;
using hime::Planet;

namespace {

class PieceTest : public ::testing::Test{};

TEST_F(PieceTest, construct) {
  Skill activeSkill("1", "全体回復",
      "味方全員を@回復する", 30);
  Skill passiveSkill("1", "癒やし",
      "周りの駒が毎ターン@ずつ回復する", 30);
  EXPECT_EQ("1", activeSkill.id_);
  EXPECT_EQ(30, passiveSkill.rate_);
  MasterPiece mp("1", "姫", Planet::kSun, activeSkill, passiveSkill,
      60, 50, 80);
  EXPECT_EQ("1", mp.id_);
  OwnedPiece op(mp, "a");
  EXPECT_EQ(60, op.master_.power_);
  SessionPiece sp(op, 1, 0, {0, 0});
  EXPECT_EQ(1, sp.id_);
  SessionContextImpl ctx(0);
  vector<vector<OwnedPiece*>> pieces = {{&op}};
  Session s(&ctx, 2, 1, 1, pieces);
}

}  // namespace

