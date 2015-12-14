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
using hime::Point;
using hime::PieceAction;

namespace {

class SessionContextStub : public SessionContext {
 public:
  int random() { return 0; }
};

class SessionTest : public testing::Test {
 protected:
  virtual void SetUp() {
    auto ps1 = make_shared<const Skill>(
        "1", "全体回復", "味方全員を@回復する", 30);
    auto ps2 = make_shared<const Skill>(
        "1", "一矢", "この駒を倒した相手に攻撃する", 0);
    auto as1 = make_shared<const Skill>(
        "1", "癒やし", "周りの駒が毎ターン@ずつ回復する", 30);
    auto as2 = make_shared<const Skill>(
        "1", "突撃", "攻撃力2倍で2マス前進", 0);
    auto mp1 = make_shared<const MasterPiece>("1", "姫", Planet::kSun,
        PieceAction::kHeal, ps1, as1, hime::Parameter(60, 50, 80));
    auto mp2 = make_shared<const MasterPiece>("1", "浪人", Planet::kMars,
        PieceAction::kPhysical, ps2, as2, hime::Parameter(80, 80, 60));
    auto op1 = make_shared<const OwnedPiece>(mp1, "a");
    auto op2 = make_shared<const OwnedPiece>(mp2, "b");
    vector<vector<shared_ptr<const OwnedPiece>>> pieces = {{op1}, {op2}};
    s_ = new Session(make_unique<SessionContextStub>(), 2, 1, 1, pieces);
  }
  virtual void TearDown() {
    delete s_;
  }
  void ExpectPoint(const Point& a, const Point& b) {
    EXPECT_EQ(a.i, b.i);
    EXPECT_EQ(a.j, b.j);
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
  s_->CommitFormation({{"a", {4, 2}}});
  auto acts = s_->ProcessTurn({{0, 0}});  // Front
  EXPECT_EQ(1, acts.size());
  auto act = unique_ptr<ActionMove>(
      static_cast<ActionMove*>(acts[0].release()));
  EXPECT_EQ(Action::Type::kMove, act->type);
  ExpectPoint({4, 2}, act->from);
  ExpectPoint({2, 2}, act->to);
  auto &p = s_->pieces()[0];
  ExpectPoint({2, 2}, p->position());
  s_->ProcessTurn({{0, 0}});  // FrontR
  ExpectPoint({1, 3}, p->position());
  s_->ProcessTurn({{0, 0}});  // FrontL
  ExpectPoint({0, 2}, p->position());
  s_->ProcessTurn({{0, 0}});  // BackR
  ExpectPoint({1, 3}, p->position());
  s_->ProcessTurn({{0, 0}});  // BackL
  ExpectPoint({2, 2}, p->position());
  s_->ProcessTurn({{0, 0}});  // Back
  ExpectPoint({4, 2}, p->position());
}

TEST_F(SessionTest, ProcessTurnInvalid) {
  s_->CommitFormation({{"a", {4, 2}}});
  auto acts = s_->ProcessTurn({{100, 0}});
  EXPECT_EQ(0, acts.size());
  acts = s_->ProcessTurn({{0, 100}});
  EXPECT_EQ(0, acts.size());
}

TEST_F(SessionTest, FindPiece) {
  s_->CommitFormation({{"a", {4, 2}}});
  EXPECT_EQ(0, s_->FindPiece({4, 2}));
  EXPECT_EQ(-1, s_->FindPiece({100, 100}));
}

TEST_F(SessionTest, CalcDamage) {
  s_->CommitFormation({{"a", {6, 2}}, {"b", {8, 2}}});
  EXPECT_EQ(128, s_->CalcDamage(1, 0));
}

TEST_F(SessionTest, CalcDamageInvalid) {
  s_->CommitFormation({{"a", {6, 2}}, {"b", {8, 2}}});
  EXPECT_EQ(0, s_->CalcDamage(100, 0));
}

}  // namespace

