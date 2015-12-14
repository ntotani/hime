#include <vector>

#include "gtest/gtest.h"

#include "hime/session.h"

using std::make_shared;
using std::make_unique;
using std::shared_ptr;
using std::unique_ptr;
using std::vector;
using hime::Action;
using hime::ActionMove;
using hime::MasterPiece;
using hime::OwnedPiece;
using hime::SessionPiece;
using hime::Skill;
using hime::Session;
using hime::SessionContext;
using hime::SessionContextImpl;
using hime::Planet;

namespace {

class SessionContextStub : public SessionContext {
 public:
  int random() { return 0; }
};

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
    s_ = new Session(make_unique<SessionContextStub>(), 2, 1, 1, pieces);
  }
  virtual void TearDown() {
    delete s_;
  }
  Session* s_;
};

TEST_F(SessionTest, Constructor) {
  EXPECT_EQ(hime::Tile::kNone, s_->board().tiles()[0][0]);
  EXPECT_EQ(hime::Card::kFront, s_->decks()[0][0]);
  EXPECT_EQ(0, s_->pieces().size());
  EXPECT_EQ(2, s_->decks().size());
  EXPECT_EQ(12, s_->decks()[0].size());
  EXPECT_EQ(2, s_->hands().size());
  EXPECT_EQ(0, s_->hands()[0].size());
  EXPECT_EQ(2, s_->trash().size());
  EXPECT_EQ(0, s_->trash()[0].size());
}

TEST_F(SessionTest, CommitFormation) {
  s_->CommitFormation({{"a", {1, 1}}});
  EXPECT_EQ(1, s_->pieces().size());
  auto &p = s_->pieces()[0];
  EXPECT_EQ("a", p->owned()->id());
  EXPECT_EQ(0, p->id());
  EXPECT_EQ(0, p->team());
  EXPECT_EQ(1, p->position().i);
  EXPECT_EQ(100, p->hp());
  EXPECT_EQ(100, p->pump().power);
  EXPECT_EQ(8, s_->decks()[0].size());
  EXPECT_EQ(4, s_->hands()[0].size());
}

TEST_F(SessionTest, ProcessTurn) {
  s_->CommitFormation({{"a", {9, 3}}});
  auto acts = s_->ProcessTurn({{0, 0}});
  EXPECT_EQ(1, acts.size());
  auto act = unique_ptr<ActionMove>(
      static_cast<ActionMove*>(acts[0].release()));
  EXPECT_EQ(Action::Type::kMove, act->type);
  EXPECT_EQ(9, act->from.i);
  EXPECT_EQ(7, act->to.i);
  auto &p = s_->pieces()[0];
  EXPECT_EQ(7, p->position().i);
}

}  // namespace

