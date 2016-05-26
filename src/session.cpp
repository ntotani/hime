#include "hime/session.h"

#include <algorithm>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <stack>
#include <exception>

using std::unordered_map;
using std::make_unique;
using std::pair;
using std::shared_ptr;
using std::string;
using std::to_string;
using std::unique_ptr;
using std::vector;
using std::stack;
using std::invalid_argument;

NS_HIME_BEGIN

const vector<Point> kDirs = {
    {-2, 0}, {-1, 1}, {1, 1}, {2, 0}, {1, -1}, {-1, -1}};

Session::Session(unique_ptr<SessionContext> context, int player_num,
    int board_id, int deck_id,
    const vector<vector<shared_ptr<const OwnedPiece>>> &pieces)
    :player_num_(player_num), owned_pieces_(pieces), context_(move(context)),
    board_(board_id) {
  switch (deck_id) {
    default:
      for (int i = 0; i < player_num; i++) {
        decks_.push_back({
            Card::kFront,
            Card::kFrontR,
            Card::kFrontL,
            Card::kBackR,
            Card::kBackL,
            Card::kBack,
            Card::kFrontFront,
            Card::kFrontRFrontR,
            Card::kFrontLFrontL,
            Card::kFrontFrontR,
            Card::kFrontFrontL,
            Card::kSkill});
      }
  }
  for (int i = 0; i < player_num; i++) {
    hands_.push_back({});
    trash_.push_back({});
    hand_size_.push_back(4);
  }
}

bool Session::CommitFormation(const unordered_map<string, Point>& formation) {
  int team_id = 0;
  int piece_id = 0;
  for (auto &pieces : owned_pieces_) {
    for (auto e : pieces) {
      if (formation.find(e->id()) != formation.end()) {
        pieces_.push_back(make_unique<SessionPiece>(e, piece_id, team_id,
              formation.at(e->id())));
        ++piece_id;
      }
    }
    ++team_id;
  }
  DrawCard();
  return false;
}

bool Session::CommitFormation(
    const unordered_map<string, SessionPieceState>& formation) {
  int team_id = 0;
  int piece_id = 0;
  for (auto &pieces : owned_pieces_) {
    for (auto e : pieces) {
      if (formation.find(e->id()) != formation.end()) {
        pieces_.push_back(make_unique<SessionPiece>(e, piece_id, team_id,
              formation.at(e->id())));
        ++piece_id;
      }
    }
    ++team_id;
  }
  DrawCard();
  return false;
}

vector<unique_ptr<Action>> Session::ProcessTurn(
    const vector<Command>& commands) {
  stack<unique_ptr<Action>> resolve;
  for (auto it = commands.rbegin(); it != commands.rend(); it++) {
    auto cmd = *it;
    if (pieces_.size() <= static_cast<size_t>(cmd.piece_id)
        || cmd.piece_id < 0) {
      throw invalid_argument("invalid piece_id: " + to_string(cmd.piece_id));
    }
    if (board_.IsOutOfBounce(cmd.to)) {
      throw invalid_argument("point out of bounce: "
          + cmd.to.ToPicoValue().serialize());
    }
    auto& actor = pieces_[cmd.piece_id];
    if (actor->hp() <= 0) {
      throw invalid_argument("dead piece_id: " + to_string(cmd.piece_id));
    }
    bool validRange = false;
    for (auto dir : actor->range()) {
      validRange |= (actor->position() + dir) == cmd.to;
    }
    if (!validRange) {
      throw invalid_argument("piece can not go to: "
          + cmd.to.ToPicoValue().serialize());
    }
    resolve.push(make_unique<ActionMove>(cmd.piece_id, cmd.to));
  }
  vector<unique_ptr<Action>> acts;
  while (!resolve.empty()) {
    auto action = move(resolve.top());
    resolve.pop();
    if (PreAction(action, &resolve)) {
      continue;
    }
    acts.push_back(move(action));
    PostAction(acts.back(), &resolve);
  }
  return move(acts);
}

void Session::ApplyActions(const vector<unique_ptr<Action>>& acts) {
  for (auto &act : acts) {
    if (act->type == Action::Type::kMove) {
      auto move = static_cast<ActionMove*>(act.get());
      pieces_[move->actor_id]->position_ = move->to;
    }
  }
}

string Session::ActsToStr(const vector<unique_ptr<Action>>& acts) {
  picojson::array arr;
  for (auto &e : acts) {
    arr.push_back(e->ToPicoValue());
  }
  return move(picojson::value(arr).serialize());
}

vector<unique_ptr<Action>> Session::StrToActs(const string& str) {
  vector<unique_ptr<Action>> acts;
  picojson::value root;
  auto error = picojson::parse(root, str);
  if (error.empty() && root.is<picojson::array>()) {
    for (auto &e : root.get<picojson::array>()) {
      acts.push_back(move(Action::FromPicoValue(e)));
    }
  }
  return move(acts);
}

int Session::FindPiece(Point position) const {
  for (auto& e : pieces_) {
    if (e->position() == position) {
      return e->id();
    }
  }
  return -1;
}

bool Session::IsHime(int piece_id) const {
  auto& actor = pieces_[piece_id];
  return actor->owned()->master()->id() == "1";
}

bool Session::IsDrop(int team_id) const {
  int hpSum = 0;
  for (auto& e : pieces_) {
    if (e->team() == team_id) {
      if (IsHime(e->id()) && e->hp() <= 0) {
        return true;
      }
      hpSum += e->hp();
    }
  }
  return hpSum <= 0;
}

int Session::CalcDamage(int actor_id, int target_id) const {
  if (pieces_.size() <= static_cast<size_t>(actor_id)
      || pieces_.size() <= static_cast<size_t>(target_id)) {
    return 0;
  }
  auto& actor = pieces_[actor_id];
  auto& target = pieces_[target_id];
  int attack = actor->param().power;
  int defense = actor->action() == PieceAction::kPhysical ?
      target->param().defense : target->param().resist;
  int pr = kPlanetRate
      [static_cast<int>(actor->planet())]
      [static_cast<int>(target->planet())];
  return std::max(40 * attack * pr / 100 / defense, 1);
}

Point Session::RotateDir(Point dir, int team) const {
  auto s = static_cast<int>(kDirs.size());
  for (int i = 0; i < s; i++) {
    if (kDirs[i] == dir) {
      return kDirs[(i + s / player_num_ * team) % s];
    }
  }
  return dir;
}

void Session::DrawCard() {
  for (int i = 0; i < player_num_; i++) {
    if (hands_[i].size() == 0 && decks_[i].size() == 0) {
      decks_[i] = trash_[i];
      trash_[i].clear();
    }
    while (hands_[i].size() < static_cast<size_t>(hand_size_[i])
        && decks_[i].size() > 0) {
      int idx = context_->random() % decks_[i].size();
      hands_[i].push_back(decks_[i][idx]);
      decks_[i].erase(decks_[i].begin() + idx);
    }
  }
}

bool Session::PreAction(
    const unique_ptr<Action>& action, stack<unique_ptr<Action>>* resolve) {
  if (action->type != Action::Type::kMove) {
    return false;
  }
  auto act = static_cast<ActionMove*>(action.get());
  auto& actor = pieces_[act->actor_id];
  int hit = FindPiece(act->to);
  if (hit != -1) {
    if (actor->action() == PieceAction::kHeal) {
      Heal(act->actor_id, hit, resolve);
    } else {
      Attack(act->actor_id, hit, resolve);
    }
    return true;
  }
  return false;
}

void Session::PostAction(
    const unique_ptr<Action>& action, stack<unique_ptr<Action>>* resolve) {
  switch (action->type) {
    case Action::Type::kMove: {
      auto act = static_cast<ActionMove*>(action.get());
      pieces_[act->actor_id]->position_ = act->to;
      auto &tile = board_.tiles()[act->to.i][act->to.j];
      if (IsHime(act->actor_id) && tile->type == Tile::Type::kCamp) {
        auto camp = static_cast<TileCamp*>(tile.get());
        if (camp->team_id != pieces_[act->actor_id]->team()) {
          resolve->push(make_unique<ActionDrop>(camp->team_id));
        }
      } else if (board_.IsOutOfBounce(act->to)) {
        pieces_[act->actor_id]->hp_ = 0;
        if (IsDrop(pieces_[act->actor_id]->team())) {
          resolve->push(
              make_unique<ActionDrop>(pieces_[act->actor_id]->team()));
        }
        resolve->push(make_unique<ActionOb>(act->actor_id, act->to));
      }
      break;}
    case Action::Type::kAttack: {
      auto attack = static_cast<ActionAttack*>(action.get());
      pieces_[attack->target_id]->hp_ =
        std::max(pieces_[attack->target_id]->hp() - attack->dmg, 0);
      if (pieces_[attack->target_id]->hp() <= 0) {
        if (IsDrop(pieces_[attack->target_id]->team())) {
          resolve->push(make_unique<ActionDrop>(
                pieces_[attack->target_id]->team()));
        }
        resolve->push(make_unique<ActionDead>(attack->target_id));
      }
      break; }
    case Action::Type::kHeal: {
      auto heal = static_cast<ActionHeal*>(action.get());
      pieces_[heal->target_id]->hp_ =
        std::min(pieces_[heal->target_id]->hp() + heal->gain, 100);
      break; }
    default:
      break;
  }
}

vector<Point> Session::Card2Dirs(Card card) {
  vector<Point> dirs;
  switch (card) {
    case Card::kFront: dirs = {{-2, 0}}; break;
    case Card::kFrontR: dirs = {{-1, 1}}; break;
    case Card::kFrontL: dirs = {{-1, -1}}; break;
    case Card::kBackR: dirs = {{1, 1}}; break;
    case Card::kBackL: dirs = {{1, -1}}; break;
    case Card::kBack: dirs = {{2, 0}}; break;
    case Card::kFrontFront: dirs = {{-2, 0}, {-2, 0}}; break;
    case Card::kFrontRFrontR: dirs = {{-1, 1}, {-1, 1}}; break;
    case Card::kFrontLFrontL: dirs = {{-1, -1}, {-1, -1}}; break;
    case Card::kFrontFrontR: dirs = {{-2, 0}, {-1, 1}}; break;
    case Card::kFrontFrontL: dirs = {{-2, 0}, {-1, -1}}; break;
    default: dirs = {};
  }
  return move(dirs);
}

void Session::Attack(
    int actor_id, int target_id, stack<unique_ptr<Action>>* resolve) {
  int damage = CalcDamage(actor_id, target_id);
  Attack(actor_id, target_id, damage, resolve);
}

void Session::Attack(
    int actor_id, int target_id, int damage,
    stack<unique_ptr<Action>>* resolve) {
  resolve->push(make_unique<ActionAttack>(actor_id, target_id, damage));
}

void Session::Heal(
    int actor_id, int target_id, stack<unique_ptr<Action>>* resolve) {
  int gain = CalcDamage(actor_id, target_id);
  Heal(actor_id, target_id, gain, resolve);
}

void Session::Heal(
    int actor_id, int target_id, int gain, stack<unique_ptr<Action>>* resolve) {
  resolve->push(make_unique<ActionHeal>(actor_id, target_id, gain));
}

SessionBuilder::SessionBuilder(
    int seed, int player_num, int board_id, int deck_id)
  :seed_(seed), player_num_(player_num), board_id_(board_id), deck_id_(deck_id),
  pieces_(vector<vector<shared_ptr<const OwnedPiece>>>(player_num)) {
}

void SessionBuilder::PushPiece(
    shared_ptr<const OwnedPiece> piece, int team) {
  pieces_[team].push_back(piece);
}

unique_ptr<Session> SessionBuilder::Build() {
  return make_unique<Session>(make_unique<SessionContextImpl>(seed_),
      player_num_, board_id_, deck_id_, pieces_);
}

NS_HIME_END

