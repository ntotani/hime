#include "hime/session.h"

#include <string>
#include <utility>
#include <vector>

using std::pair;
using std::string;
using std::vector;

NS_HIME_BEGIN

Session::Session(SessionContext *context, int player_num,
    int board_id, int deck_id, const vector<vector<const OwnedPiece*>> &pieces)
    :player_num_(player_num), owned_pieces_(pieces), context_(*context),
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

bool Session::CommitFormation(const vector<pair<string, Point>> &formation) {
  return false;
}

NS_HIME_END

