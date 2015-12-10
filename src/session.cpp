#include "hime/session.h"

#include <map>
#include <string>
#include <utility>
#include <vector>

using std::map;
using std::pair;
using std::string;
using std::unique_ptr;
using std::vector;

NS_HIME_BEGIN

Session::Session(unique_ptr<SessionContext> context, int player_num,
    int board_id, int deck_id, const vector<vector<const OwnedPiece*>> &pieces)
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

bool Session::CommitFormation(const map<string, Point> &formation) {
  int team_id = 0;
  int piece_id = 0;
  for (auto &pieces : owned_pieces_) {
    for (auto e : pieces) {
      if (formation.find(e->id_) != formation.end()) {
        pieces_.emplace_back(new SessionPiece(*e, ++piece_id, team_id,
              formation.at(e->id_)));
      }
    }
    ++team_id;
  }
  return false;
}

NS_HIME_END

