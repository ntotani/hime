#include "hime/session.h"

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
    // TODO(ntotani): validate no actor
    auto card = hands_[pieces_[cmd.piece_id]->team()][cmd.card_idx];
    for (auto dir : Card2Dirs(card)) {
      auto e = MovePiece(cmd.piece_id, dir);
      acts.insert(acts.end(),
          make_move_iterator(e.begin()), make_move_iterator(e.end()));
    }
  }
  return move(acts);
}

void Session::DrawCard() {
  for (int i = 0; i < player_num_; i++) {
    if (hands_[i].size() == 0 && decks_[i].size() == 0) {
      decks_[i] = trash_[i];
      trash_[i].clear();
    }
    while (hands_[i].size() < hand_size_[i] && decks_[i].size() > 0) {
      int idx = context_->random() % decks_[i].size();
      hands_[i].push_back(decks_[i][idx]);
      decks_[i].erase(decks_[i].begin() + idx);
    }
  }
}

vector<unique_ptr<Action>> Session::MovePiece(int piece_id, Point dir) {
  vector<unique_ptr<Action>> acts;
  auto from = pieces_[piece_id]->position();
  pieces_[piece_id]->MoveBy(dir);
  acts.push_back(make_unique<ActionMove>(
      piece_id, from, pieces_[piece_id]->position()));
  return move(acts);
}

vector<Point> Session::Card2Dirs(Card card) {
  vector<Point> dirs;
  switch (card) {
    case Card::kFront: dirs = {{-2, 0}}; break;
    default: dirs = {};
  }
  return move(dirs);
}

NS_HIME_END

