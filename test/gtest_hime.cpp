#include <vector>
#include <exception>

#include "gtest/gtest.h"

#include "hime/session.h"
#include "hime/master.h"

using std::make_shared;
using std::make_unique;
using std::shared_ptr;
using std::unique_ptr;
using std::vector;
using std::invalid_argument;
using std::domain_error;
using hime::Action;
using hime::ActionChip;
using hime::ActionMove;
using hime::ActionOb;
using hime::ActionAttack;
using hime::ActionHeal;
using hime::ActionDead;
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
    master.LoadSkill("a0,dummy,dummy skill\n"
      "a1,全体回復,味方全員を@回復する\n"
      "a3,突撃,攻撃力2倍で2マス前進\n"
      "p0,dummy,dummy skill\n"
      "p1,癒やし,周りの駒が毎ターン@ずつ回復する\n"
      "p3,一矢,この駒を倒した相手に攻撃する");
    master.LoadPiece("0,dummy,mar,phys,p0,40,40,40,abcf\n"
        "1,姫,sun,heal,p1,60,50,80,abcf\n"
        "3,浪人,mar,phys,p3,80,80,60,abcf");
  }
  hime::Master master;
};
HimeEnv* env = static_cast<HimeEnv*>(
    testing::AddGlobalTestEnvironment(new HimeEnv));

class HimeTest : public testing::Test {
 protected:
  void ExpectPoint(const Point& a, const Point& b) {
    EXPECT_EQ(a.i, b.i);
    EXPECT_EQ(a.j, b.j);
  }
};

class PieceTest : public HimeTest {};

TEST_F(PieceTest, Constructor) {
  auto mp = env->master.piece("0");
  EXPECT_EQ("0", mp->id());
  EXPECT_EQ("dummy", mp->name());
  EXPECT_EQ(Planet::kMars, mp->planet());
  EXPECT_EQ(PieceAction::kPhysical, mp->action());
  EXPECT_EQ(40, mp->param().power);
  EXPECT_EQ(40, mp->param().defense);
  EXPECT_EQ(40, mp->param().resist);
  auto range = mp->range();
  ExpectPoint({-2, 0}, range[0]);
  ExpectPoint({-1, 1}, range[1]);
  ExpectPoint({-1, -1}, range[2]);
  ExpectPoint({2, 0}, range[3]);
  auto ps = mp->passive_skill();
  EXPECT_EQ("p0", ps->id());
  EXPECT_EQ("dummy", ps->name());
  EXPECT_EQ("dummy skill", ps->desc());
}

class SessionContextStub : public SessionContext {
 public:
  int random() { return 0; }
};

class SessionTest : public HimeTest {
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
  ExpectPoint({2, 2}, act->to);
  EXPECT_EQ("{\"actor_id\":0,\"to\":{\"i\":2,\"j\":2},\"type\":\"move\"}",
      act->ToPicoValue().serialize());
  auto &p = s_->pieces()[0];
  ExpectPoint({2, 2}, p->position());
}

TEST_F(SessionTest, ProcessTurnInvalid) {
  try {
    s_->ProcessTurn({{0, {2, 2}}});
    FAIL();
  } catch (const domain_error& e) {
    EXPECT_STREQ("cant ProcessTurn before CommitFormation", e.what());
  }
  s_->CommitFormation({{"a", {4, 2}}});
  try {
    s_->ProcessTurn({{100, {2, 2}}});
    FAIL();
  } catch (const invalid_argument& e) {
    EXPECT_STREQ("invalid piece_id: 100", e.what());
  }
  try {
    s_->ProcessTurn({{-1, {2, 2}}});
    FAIL();
  } catch (const invalid_argument& e) {
    EXPECT_STREQ("invalid piece_id: -1", e.what());
  }
  try {
    s_->ProcessTurn({{0, {100, 2}}});
    FAIL();
  } catch (const invalid_argument& e) {
    EXPECT_STREQ("point out of bounce: {\"i\":100,\"j\":2}", e.what());
  }
  try {
    s_->ProcessTurn({{0, {0, 2}}});
    FAIL();
  } catch (const invalid_argument& e) {
    EXPECT_STREQ("piece can not go to: {\"i\":0,\"j\":2}", e.what());
  }
}

TEST_F(SessionTest, ProcessTurnInvalidDead) {
  s_->CommitFormation({{"a", {{4, 2}, 0, Parameter(0)}}});
  try {
    s_->ProcessTurn({{0, {2, 2}}});
    FAIL();
  } catch (const invalid_argument& e) {
    EXPECT_STREQ("dead piece_id: 0", e.what());
  }
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

TEST_F(SessionTest, ProcessTurnAttack) {
  s_->CommitFormation({{"a", {8, 2}}, {"b", {6, 2}}});
  auto acts = s_->ProcessTurn({{0, {6, 2}}});
  EXPECT_EQ(1, acts.size());
  auto act = unique_ptr<ActionAttack>(
      static_cast<ActionAttack*>(acts[0].release()));
  EXPECT_EQ(Action::Type::kAttack, act->type);
  EXPECT_EQ(0, act->actor_id);
  EXPECT_EQ(1, act->target_id);
  EXPECT_EQ(40, act->dmg);
  EXPECT_EQ("{\"actor_id\":0,\"dmg\":40,\"target_id\":1,\"type\":\"attack\"}",
      act->ToPicoValue().serialize());
}

TEST_F(SessionTest, ApplyActions) {
  s_->CommitFormation({{"a", {8, 2}}});
  Point to = {6, 2};
  vector<unique_ptr<Action>> acts;
  acts.push_back(make_unique<ActionMove>(0, to));
  s_->ApplyActions(acts);
  auto &p = s_->pieces()[0];
  ExpectPoint({6, 2}, p->position());
}

class SessionHimeTest : public HimeTest {
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
  Session* s_;
};

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
  EXPECT_EQ(3, acts.size());
  auto dead = unique_ptr<ActionDead>(
      static_cast<ActionDead*>(acts[1].release()));
  EXPECT_EQ(Action::Type::kDead, dead->type);
  EXPECT_EQ(0, dead->actor_id);
  auto drop = unique_ptr<ActionDrop>(
      static_cast<ActionDrop*>(acts[2].release()));
  EXPECT_EQ(Action::Type::kDrop, drop->type);
  EXPECT_EQ(0, drop->team_id);
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
  EXPECT_EQ(60, act->gain);
  EXPECT_EQ(61, s_->pieces()[1]->hp());
  EXPECT_EQ("{\"actor_id\":0,\"gain\":60,\"target_id\":1,\"type\":\"heal\"}",
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

class SessionStringifyTest : public HimeTest {};

TEST_F(SessionStringifyTest, ActionChipMove) {
  auto json = "[{\"actor_id\":1,\"chip_idx\":1,\"type\":\"chip\"},"
    "{\"actor_id\":2,\"to\":{\"i\":2,\"j\":2},\"type\":\"move\"}]";
  auto acts = Session::StrToActs(json);
  EXPECT_EQ(2, acts.size());
  auto chip = static_cast<ActionChip*>(acts[0].get());
  EXPECT_EQ(1, chip->actor_id);
  EXPECT_EQ(1, chip->chip_idx);
  auto move = static_cast<ActionMove*>(acts[1].get());
  EXPECT_EQ(2, move->actor_id);
  ExpectPoint({2, 2}, move->to);
  EXPECT_EQ(json, Session::ActsToStr(acts));
}

TEST_F(SessionStringifyTest, ActionOb) {
  auto json = "[{\"actor_id\":1,\"pos\":{\"i\":-2,\"j\":2},\"type\":\"ob\"}]";
  auto acts = Session::StrToActs(json);
  auto ob = static_cast<ActionOb*>(acts[0].get());
  EXPECT_EQ(1, ob->actor_id);
  ExpectPoint({-2, 2}, ob->pos);
  EXPECT_EQ(json, Session::ActsToStr(acts));
}

TEST_F(SessionStringifyTest, ActionAttack) {
  auto json = "[{\"actor_id\":1,\"dmg\":1,"
      "\"target_id\":2,\"type\":\"attack\"}]";
  auto acts = Session::StrToActs(json);
  auto attack = static_cast<ActionAttack*>(acts[0].get());
  EXPECT_EQ(1, attack->actor_id);
  EXPECT_EQ(1, attack->dmg);
  EXPECT_EQ(2, attack->target_id);
  EXPECT_EQ(json, Session::ActsToStr(acts));
}

TEST_F(SessionStringifyTest, ActionHeal) {
  auto json = "[{\"actor_id\":1,\"gain\":1,\"target_id\":2,\"type\":\"heal\"}]";
  auto acts = Session::StrToActs(json);
  auto heal = static_cast<ActionHeal*>(acts[0].get());
  EXPECT_EQ(1, heal->actor_id);
  EXPECT_EQ(1, heal->gain);
  EXPECT_EQ(2, heal->target_id);
  EXPECT_EQ(json, Session::ActsToStr(acts));
}

TEST_F(SessionStringifyTest, ActionDead) {
  auto json = "[{\"actor_id\":1,\"type\":\"dead\"}]";
  auto acts = Session::StrToActs(json);
  auto dead = static_cast<ActionDead*>(acts[0].get());
  EXPECT_EQ(1, dead->actor_id);
  EXPECT_EQ(json, Session::ActsToStr(acts));
}

TEST_F(SessionStringifyTest, ActionDrop) {
  auto json = "[{\"team_id\":1,\"type\":\"drop\"}]";
  auto acts = Session::StrToActs(json);
  auto drop = static_cast<ActionDrop*>(acts[0].get());
  EXPECT_EQ(1, drop->team_id);
  EXPECT_EQ(json, Session::ActsToStr(acts));
}

}  // namespace

