#include <vector>

#include "gtest/gtest.h"

#include "hime/session.h"
#include "hime/master.h"

using std::make_shared;
using std::make_unique;
using std::shared_ptr;
using std::unique_ptr;
using std::vector;
using hime::Action;
using hime::ActionChip;
using hime::ActionMove;
using hime::ActionOb;
using hime::ActionAttack;
using hime::ActionHeal;
using hime::ActionDrop;
using hime::MasterPiece;
using hime::OwnedPiece;
using hime::SessionPiece;
using hime::Skill;
using hime::Session;
using hime::SessionBuilder;
using hime::SessionContext;
using hime::SessionContextImpl;
using hime::Planet;
using hime::Point;
using hime::PieceAction;
using hime::Parameter;

namespace {

class HimeEnv : public testing::Environment {
 public:
  virtual void SetUp() {
    master.LoadSkill("a0,dummy,dummy skill,0\n"
      "a1,全体回復,味方全員を@回復する,30\n"
      "a3,突撃,攻撃力2倍で2マス前進,0\n"
      "p0,dummy,dummy skill,0\n"
      "p1,癒やし,周りの駒が毎ターン@ずつ回復する,30\n"
      "p3,一矢,この駒を倒した相手に攻撃する,0");
    master.LoadPiece("0,dummy,mar,phys,a0,p0,40,40,40\n"
        "1,姫,sun,heal,a1,p1,60,50,80\n"
        "3,浪人,mar,phys,a3,p3,80,80,60");
  }
  hime::Master master;
};
HimeEnv* env = static_cast<HimeEnv*>(
    testing::AddGlobalTestEnvironment(new HimeEnv));

class PieceTest : public testing::Test {};

TEST_F(PieceTest, Constructor) {
  EXPECT_EQ("姫", env->master.piece("1")->name());
}

class SessionContextStub : public SessionContext {
 public:
  int random() { return 0; }
};

class SessionTest : public testing::Test {
 protected:
  virtual void SetUp() {
    auto op1 = make_shared<const OwnedPiece>(env->master.piece("0"), "a");
    auto op2 = make_shared<const OwnedPiece>(env->master.piece("0"), "b");
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
  EXPECT_EQ(hime::Tile::Type::kNone, s_->board().tiles()[0][0]->type);
  EXPECT_EQ(hime::Card::kFront, s_->decks()[0][0]);
  EXPECT_EQ(0, s_->pieces().size());
  EXPECT_EQ(2, s_->decks().size());
  EXPECT_EQ(12, s_->decks()[0].size());
  EXPECT_EQ(2, s_->hands().size());
  EXPECT_EQ(0, s_->hands()[0].size());
  EXPECT_EQ(2, s_->trash().size());
  EXPECT_EQ(0, s_->trash()[0].size());
}

TEST_F(SessionTest, Builder) {
  SessionBuilder builder(0, 2, 1, 1);
  builder.PushPiece(s_->owned_pieces()[0][0], 0);
  builder.PushPiece(s_->owned_pieces()[1][0], 1);
  auto s = builder.Build();
  EXPECT_EQ(hime::Tile::Type::kNone, s->board().tiles()[0][0]->type);
  EXPECT_EQ(hime::Card::kFront, s->decks()[0][0]);
  EXPECT_EQ(0, s->pieces().size());
  EXPECT_EQ(2, s->decks().size());
  EXPECT_EQ(12, s->decks()[0].size());
  EXPECT_EQ(2, s->hands().size());
  EXPECT_EQ(0, s->hands()[0].size());
  EXPECT_EQ(2, s->trash().size());
  EXPECT_EQ(0, s->trash()[0].size());
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
  auto acts = s_->ProcessTurn({{0, {2, 2}}});  // Front
  auto act = unique_ptr<ActionMove>(
      static_cast<ActionMove*>(acts[0].release()));
  EXPECT_EQ(Action::Type::kMove, act->type);
  ExpectPoint({4, 2}, act->from);
  ExpectPoint({2, 2}, act->to);
  EXPECT_EQ("{\"actor_id\":0,\"from\":{\"i\":4,\"j\":2},"
      "\"to\":{\"i\":2,\"j\":2},\"type\":\"move\"}",
      act->ToPicoValue().serialize());
  auto &p = s_->pieces()[0];
  ExpectPoint({2, 2}, p->position());
}

TEST_F(SessionTest, ProcessTurnInvalid) {
  s_->CommitFormation({{"a", {4, 2}}});
  auto acts = s_->ProcessTurn({{100, {2, 2}}});
  EXPECT_EQ(0, acts.size());
}

TEST_F(SessionTest, FindPiece) {
  s_->CommitFormation({{"a", {4, 2}}});
  EXPECT_EQ(0, s_->FindPiece({4, 2}));
  EXPECT_EQ(-1, s_->FindPiece({100, 100}));
}

TEST_F(SessionTest, CalcDamage) {
  s_->CommitFormation({{"a", {6, 2}}, {"b", {8, 2}}});
  EXPECT_EQ(40, s_->CalcDamage(1, 0));
}

TEST_F(SessionTest, CalcDamageInvalid) {
  s_->CommitFormation({{"a", {6, 2}}, {"b", {8, 2}}});
  EXPECT_EQ(0, s_->CalcDamage(100, 0));
}

TEST_F(SessionTest, RotateDir) {
  ExpectPoint({2, 0}, s_->RotateDir({-2, 0}, 1));
  ExpectPoint({1, -1}, s_->RotateDir({-1, 1}, 1));
}

TEST_F(SessionTest, ProcessTurnOb) {
  s_->CommitFormation({{"a", {0, 2}}});
  auto acts = s_->ProcessTurn({{0, {-2, 2}}});
  EXPECT_EQ(2, acts.size());
  auto act = unique_ptr<ActionOb>(
      static_cast<ActionOb*>(acts[0].release()));
  EXPECT_EQ(Action::Type::kOb, act->type);
  ExpectPoint({-2, 2}, act->pos);
  EXPECT_EQ("{\"actor_id\":0,\"pos\":{\"i\":-2,\"j\":2},\"type\":\"ob\"}",
      act->ToPicoValue().serialize());
  auto &p = s_->pieces()[0];
  EXPECT_EQ(0, p->hp());
  auto drop = unique_ptr<ActionDrop>(
      static_cast<ActionDrop*>(acts[1].release()));
  EXPECT_EQ(Action::Type::kDrop, drop->type);
  EXPECT_EQ(0, drop->team_id);
  EXPECT_EQ("{\"team_id\":0,\"type\":\"drop\"}",
      drop->ToPicoValue().serialize());
}

TEST_F(SessionTest, ProcessTurnAttack) {
  s_->CommitFormation({{"a", {8, 2}}, {"b", {6, 2}}});
  auto acts = s_->ProcessTurn({{0, {6, 2}}});
  EXPECT_EQ(1, acts.size());
  auto act = unique_ptr<ActionAttack>(
      static_cast<ActionAttack*>(acts[0].release()));
  EXPECT_EQ(Action::Type::kAttack, act->type);
  EXPECT_EQ(0, act->actor_id);
  EXPECT_EQ(1, act->target_id);
  ExpectPoint({8, 2}, act->from);
  ExpectPoint({6, 2}, act->to);
  EXPECT_EQ(100, act->hp);
  EXPECT_EQ(40, act->dmg);
  auto &p = s_->pieces()[1];
  EXPECT_EQ(60, p->hp());
  EXPECT_EQ("{\"actor_id\":0,\"dmg\":40,\"from\":{\"i\":8,\"j\":2},\"hp\":100,"
      "\"target_id\":1,\"to\":{\"i\":6,\"j\":2},\"type\":\"attack\"}",
      act->ToPicoValue().serialize());
}

TEST_F(SessionTest, ApplyActions) {
  s_->CommitFormation({{"a", {8, 2}}});
  Point from = {8, 2};
  Point to = {6, 2};
  vector<unique_ptr<Action>> acts;
  acts.push_back(make_unique<ActionMove>(0, from, to));
  s_->ApplyActions(acts);
  auto &p = s_->pieces()[0];
  ExpectPoint({6, 2}, p->position());
}

class SessionHimeTest : public testing::Test {
 protected:
  virtual void SetUp() {
    auto op1 = make_shared<const OwnedPiece>(env->master.piece("1"), "a");
    auto op2 = make_shared<const OwnedPiece>(env->master.piece("0"), "b");
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

TEST_F(SessionHimeTest, Drop) {
  s_->CommitFormation({{"a", {0, 2}}});
  auto acts = s_->ProcessTurn({{0, {-2, 2}}});
  EXPECT_EQ(2, acts.size());
  auto act = unique_ptr<ActionDrop>(
      static_cast<ActionDrop*>(acts[1].release()));
  EXPECT_EQ(Action::Type::kDrop, act->type);
  EXPECT_EQ(0, act->team_id);
}

TEST_F(SessionHimeTest, Camp) {
  s_->CommitFormation({{"a", {2, 2}}});
  auto acts = s_->ProcessTurn({{0, {0, 2}}});
  EXPECT_EQ(2, acts.size());
  auto act = unique_ptr<ActionDrop>(
      static_cast<ActionDrop*>(acts[1].release()));
  EXPECT_EQ(Action::Type::kDrop, act->type);
  EXPECT_EQ(1, act->team_id);
}

TEST_F(SessionHimeTest, HimeDead) {
  s_->CommitFormation({
      {"a", {{8, 2},   1, Parameter(100)}},
      {"b", {{6, 2}, 100, Parameter(100)}}});
  auto acts = s_->ProcessTurn({{1, {8, 2}}});
  EXPECT_EQ(2, acts.size());
  auto act = unique_ptr<ActionDrop>(
      static_cast<ActionDrop*>(acts[1].release()));
  EXPECT_EQ(Action::Type::kDrop, act->type);
  EXPECT_EQ(0, act->team_id);
}

TEST_F(SessionHimeTest, HimeHeal) {
  s_->CommitFormation({
      {"a", {{8, 2}, 100, Parameter(100)}},
      {"b", {{6, 2},   1, Parameter(100)}}});
  auto acts = s_->ProcessTurn({{0, {6, 2}}});
  auto act = unique_ptr<ActionHeal>(
      static_cast<ActionHeal*>(acts[0].release()));
  EXPECT_EQ(Action::Type::kHeal, act->type);
  EXPECT_EQ(0, act->actor_id);
  EXPECT_EQ(1, act->target_id);
  ExpectPoint({8, 2}, act->from);
  ExpectPoint({6, 2}, act->to);
  EXPECT_EQ(1, act->hp);
  EXPECT_EQ(60, act->gain);
  EXPECT_EQ(61, s_->pieces()[1]->hp());
  EXPECT_EQ("{\"actor_id\":0,\"from\":{\"i\":8,\"j\":2},\"gain\":60,\"hp\":1,"
      "\"target_id\":1,\"to\":{\"i\":6,\"j\":2},\"type\":\"heal\"}",
      act->ToPicoValue().serialize());
}

class SessionThreeTest : public testing::Test {
 protected:
  virtual void SetUp() {
    auto op1 = make_shared<const OwnedPiece>(env->master.piece("1"), "a");
    auto op2 = make_shared<const OwnedPiece>(env->master.piece("3"), "b");
    auto op3 = make_shared<const OwnedPiece>(env->master.piece("1"), "c");
    vector<vector<shared_ptr<const OwnedPiece>>> pieces = {{op1}, {op2}, {op3}};
    s_ = new Session(make_unique<SessionContextStub>(), 3, 1, 1, pieces);
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

TEST_F(SessionThreeTest, RotateDir) {
  ExpectPoint({1, 1}, s_->RotateDir({-2, 0}, 1));
  ExpectPoint({2, 0}, s_->RotateDir({-1, 1}, 1));
}

class SessionStringifyTest : public testing::Test {};

TEST_F(SessionStringifyTest, ActionChip) {
  auto json = "[{\"actor_id\":1,\"chip_idx\":1,\"type\":\"chip\"}]";
  auto acts = Session::StrToActs(json);
  EXPECT_EQ(1, acts.size());
  auto act = static_cast<ActionChip*>(acts[0].get());
  EXPECT_EQ(1, act->actor_id);
  EXPECT_EQ(1, act->chip_idx);
  EXPECT_EQ(json, Session::ActsToStr(acts));
}

}  // namespace

