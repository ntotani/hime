#include <vector>

#include "gtest/gtest.h"

#include "hime/session.h"

using std::make_shared;
using std::make_unique;
using std::shared_ptr;
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

class SessionTest : public testing::Test {
 protected:
  virtual void SetUp() {
    auto s1 = make_shared<const Skill>(
        "1", "全体回復", "味方全員を@回復する", 30);
    auto s2 = make_shared<const Skill>(
        "1", "癒やし", "周りの駒が毎ターン@ずつ回復する", 30);
    auto mp = make_shared<const MasterPiece>("1", "姫", Planet::kSun,
        s1, s2, hime::Parameter(60, 50, 80));
    auto op = make_shared<const OwnedPiece>(mp, "a");
    vector<vector<shared_ptr<const OwnedPiece>>> pieces = {{op}};
    s_ = new Session(make_unique<SessionContextImpl>(0), 2, 1, 1, pieces);
  }
  virtual void TearDown() {
    delete s_;
  }
  Session* s_;
};

TEST_F(SessionTest, Constructor) {
  EXPECT_EQ(hime::Tile::kNone, s_->board().tiles()[0][0]);
  EXPECT_EQ(hime::Card::kFront, s_->decks()[0][0]);
}

}  // namespace

