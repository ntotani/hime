#include "hime/session.h"

#include <algorithm>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using std::unordered_map;
using std::make_unique;
using std::pair;
using std::shared_ptr;
using std::string;
using std::unique_ptr;
using std::vector;

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

vector<unique_ptr<Action>> Session::ProcessTurn(
    const vector<Command>& commands) {
  vector<unique_ptr<Action>> acts;
  for (auto cmd : commands) {
    if (pieces_.size() <= static_cast<size_t>(cmd.piece_id)) continue;
    auto& actor = pieces_[cmd.piece_id];
    auto& hands = hands_[actor->team()];
    if (hands.size() <= static_cast<size_t>(cmd.card_idx)) continue;
    auto card = hands[cmd.card_idx];
    // TODO(ntotani): chip action
    hands.erase(hands.begin() + cmd.card_idx);
    trash_[actor->team()].push_back(card);
    if (card == Card::kSkill) {
      // TODO(ntotani): parse skill
    } else {
      // TODO(ntotani): pSkill move twice
      for (auto dir : Card2Dirs(card)) {
        auto e = ApplyDir(cmd.piece_id, dir);
        acts.insert(acts.end(),
            make_move_iterator(e.begin()), make_move_iterator(e.end()));
      }
    }
  }
  DrawCard();
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
  dir = RotateDir(dir, actor->team());
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
      // TODO(ntotani): heal
    } else {
      // TODO(ntotani): attack
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
    // TODO(ntotani): check is hime
    acts.push_back(make_unique<ActionOb>(piece_id, position));
    pieces_[piece_id]->hp_ = 0;
    /*
      if self:isHime(actor) or not us.any(self.charas, function(e) return e.team == actor.team and e.hp > 0 end) then
          acts[#acts + 1] = {type = "end", lose = actor.team}
      end
    */
    return move(acts);
  }
  // TODO(ntotani): return if other piece
  /*
  if self:findChara(di, dj) then
      return true
  end
  */
  acts.push_back(make_unique<ActionMove>(
      piece_id, pieces_[piece_id]->position(), position));
  auto ret = CommitMove(piece_id, position);
  acts.insert(acts.end(),
      make_move_iterator(ret.begin()), make_move_iterator(ret.end()));
  return move(acts);
}

vector<unique_ptr<Action>> Session::CommitMove(int piece_id, Point position) {
  pieces_[piece_id]->position_ = position;
  vector<unique_ptr<Action>> acts;
  // TODO(ntotani): check actor is hime
  /*
  if self:isHime(actor) then
      if self.tiles[di][dj] == Shogi.BLUE_CAMP and actor.team == "red" then
          acts[#acts + 1] = {type = "end", lose = "blue"}
          return true
      elseif self.tiles[di][dj] == Shogi.RED_CAMP and actor.team == "blue" then
          acts[#acts + 1] = {type = "end", lose = "red"}
          return true
      end
  end
  */
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
  // TODO(ntotani): impl
  return {};
}

NS_HIME_END

