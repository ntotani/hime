#include "hime/session.h"

#include <algorithm>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <exception>

using std::unordered_map;
using std::make_unique;
using std::pair;
using std::shared_ptr;
using std::string;
using std::to_string;
using std::unique_ptr;
using std::vector;
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
  vector<unique_ptr<Action>> acts;
  for (auto cmd : commands) {
    if (pieces_.size() <= static_cast<size_t>(cmd.piece_id)
        || cmd.piece_id < 0) {
      throw invalid_argument("invalid piece_id: " + to_string(cmd.piece_id));
    }
    if (board_.IsOutOfBounce(cmd.to)) {
      throw invalid_argument("point out of bounce: "
          + cmd.to.ToPicoValue().serialize());
    }
    auto& actor = pieces_[cmd.piece_id];
    bool validRange = false;
    for (auto dir : actor->range()) {
      validRange |= (actor->position() + dir) == cmd.to;
    }
    if (!validRange) {
      throw invalid_argument("piece can not go to: "
          + cmd.to.ToPicoValue().serialize());
    }
    auto e = ApplyDir(cmd.piece_id, cmd.to - actor->position());
    acts.insert(acts.end(),
        make_move_iterator(e.begin()), make_move_iterator(e.end()));
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

vector<unique_ptr<Action>> Session::ApplyDir(int piece_id, Point dir) {
  vector<unique_ptr<Action>> acts;
  auto& actor = pieces_[piece_id];
  auto dst = actor->position() + dir;
  int hit = FindPiece(dst);
  // TODO(ntotani): sniper skill
  /*
    if (friend.pskill == "5" or friend.pskill == "6") and not hit then
        for i = 1, us.findWhere(PSKILL, {id = friend.pskill}).at do
            hit = self:findChara(ni + di * i, nj + dj * i)
            if hit then
                table.insert(acts, {type = "pskill", actor = friend.id, id = friend.pskill})
                break
            end
        end
    end
  */
  vector<unique_ptr<Action>> ret;
  if (hit != -1) {
    if (actor->action() == PieceAction::kHeal) {
      ret = Heal(piece_id, hit);
    } else {
      ret = Attack(piece_id, hit);
    }
  } else {
    ret = TryMove(piece_id, pieces_[piece_id]->position() + dir);
  }
  acts.insert(acts.end(),
      make_move_iterator(ret.begin()), make_move_iterator(ret.end()));
  return move(acts);
}

vector<unique_ptr<Action>> Session::TryMove(int piece_id, Point position) {
  vector<unique_ptr<Action>> acts;
  if (board_.IsOutOfBounce(position)) {
    acts.push_back(make_unique<ActionOb>(piece_id, position));
    pieces_[piece_id]->hp_ = 0;
    if (IsDrop(pieces_[piece_id]->team())) {
      acts.push_back(make_unique<ActionDrop>(pieces_[piece_id]->team()));
    }
    return move(acts);
  }
  // TODO(ntotani): return if other piece
  /*
  if self:findChara(di, dj) then
      return true
  end
  */
  acts.push_back(make_unique<ActionMove>(piece_id, position));
  auto ret = CommitMove(piece_id, position);
  acts.insert(acts.end(),
      make_move_iterator(ret.begin()), make_move_iterator(ret.end()));
  return move(acts);
}

vector<unique_ptr<Action>> Session::CommitMove(int piece_id, Point position) {
  pieces_[piece_id]->position_ = position;
  vector<unique_ptr<Action>> acts;
  auto &tile = board_.tiles()[position.i][position.j];
  if (IsHime(piece_id) && tile->type == Tile::Type::kCamp) {
    auto camp = static_cast<TileCamp*>(tile.get());
    if (camp->team_id != pieces_[piece_id]->team()) {
      acts.push_back(make_unique<ActionDrop>(camp->team_id));
    }
  }
  // TODO(ntotani): check evo
  /*
  if (self.tiles[di][dj] == Shogi.BLUE_EVO and actor.team == "red" or
      self.tiles[di][dj] == Shogi.RED_EVO and actor.team == "blue") and actor.evo then
      local evo = us.findWhere(CHARAS, {id = actor.evo})
      table.insert(acts, {type = "evo", actor = actor.id, from = actor.master.id, to = evo.id})
      setmetatable(actor, {__index = evo})
      actor.master = evo
  end
  */
  return move(acts);
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

vector<unique_ptr<Action>> Session::Attack(int actor_id, int target_id) {
  int damage = CalcDamage(actor_id, target_id);
  return move(Attack(actor_id, target_id, damage));
}

vector<unique_ptr<Action>> Session::Attack(
    int actor_id, int target_id, int damage) {
  vector<unique_ptr<Action>> acts;
  // TODO(ntotani): skill 仁王
  /*
  for _, e in ipairs(self:getDirs(target.team)) do
      local c = self:findChara(target.i + e.i, target.j + e.j)
      if c and c.pskill == "10" and c.team == target.team and c.id ~= actor.id then
          table.insert(acts, {type = "pskill", actor = c.id, id = "10"})
          table.insert(acts, {
              type = "swap",
              actor = c.id,
              target = target.id,
              fi = c.i,
              fj = c.j,
              ti = target.i,
              tj = target.j,
          })
          local ti, tj = target.i, target.j
          if self:commitMove(target, c.i, c.j, acts) then return end
          if self:commitMove(c, ti, tj, acts) then return end
          target = c
          break
      end
  end
  */
  // TODO(ntotani): skill 保険
  /*
  if target.pskill == "7" and target.hp >= 100 and dmg > target.hp then
      table.insert(acts, {type = "pskill", actor = target.id, id = "7"})
      dmg = 99
  end
  */
  acts.push_back(make_unique<ActionAttack>(actor_id, target_id, damage));
  pieces_[target_id]->hp_ = std::max(pieces_[target_id]->hp() - damage, 0);
  if (pieces_[target_id]->hp() <= 0) {
    if (IsDrop(pieces_[target_id]->team())) {
      acts.push_back(make_unique<ActionDrop>(pieces_[target_id]->team()));
    }
    /*
        elseif self:isNextTo(actor, target) then
            if target.pskill == "3" and actor.hp > 0 then
                table.insert(acts, {type = "pskill", actor = target.id, id = "3"})
                self:attack(target, actor, nil, acts)
            end
            if actor.pskill == "4" and actor.hp > 0 then
                table.insert(acts, {type = "pskill", actor = actor.id, id = "4"})
                actor.pump.power = actor.pump.power * 1.5
            end
            if actor.hp > 0 then
                self:moveTo(actor, target.i, target.j, acts)
            end
        end
        */
  }
  /*
    elseif target.pskill == "9" and self:isNextTo(target, actor) then
        table.insert(acts, {type = "pskill", actor = target.id, id = "9"})
        self:moveTo(actor, actor.i + (actor.i - target.i), actor.j + (actor.j - target.j), acts)
    end
  return {};
  */
  return move(acts);
}

vector<unique_ptr<Action>> Session::Heal(int actor_id, int target_id) {
  int gain = CalcDamage(actor_id, target_id);
  return move(Heal(actor_id, target_id, gain));
}

vector<unique_ptr<Action>> Session::Heal(
    int actor_id, int target_id, int gain) {
  vector<unique_ptr<Action>> acts;
  acts.push_back(make_unique<ActionHeal>(actor_id, target_id, gain));
  pieces_[target_id]->hp_ = std::min(pieces_[target_id]->hp() + gain, 100);
  return move(acts);
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

