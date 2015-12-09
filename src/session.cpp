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
    :player_num_(player_num), owned_pieces_(pieces), context_(*context) {
  // board from id
  // deck from id
}

bool Session::CommitFormation(const vector<pair<string, Point>> &formation) {
  return false;
}

NS_HIME_END

