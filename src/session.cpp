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
}

bool Session::CommitFormation(const unordered_map<string, Point> &formation) {
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
  return false;
}

unique_ptr<vector<Action>> Session::ProcessTurn(
    const vector<Command>& commands) {
  vector<Action> acts;
  acts.push_back(ActionMiss(0, 0));
  return make_unique<vector<Action>>(acts);
}

NS_HIME_END

