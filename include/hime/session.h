#ifndef INCLUDE_HIME_SESSION_H_
#define INCLUDE_HIME_SESSION_H_

#include <functional>
#include <random>
#include <string>
#include <utility>
#include <vector>

#include "hime/piece.h"

NS_HIME_BEGIN

class SessionContext {
 public:
  virtual ~SessionContext() {}
  virtual int random() = 0;
};

class SessionContextImpl : public SessionContext {
 public:
  explicit SessionContextImpl(int seed):rnd_(std::mt19937(seed)) {}
  int random() { return rnd_(); }
 private:
  std::mt19937 rnd_;
  DISALLOW_COPY_AND_ASSIGN(SessionContextImpl);
};

class Session {
 public:
  const int player_num_;
  const std::vector<std::vector<OwnedPiece*>> owned_pieces_;
  Session(SessionContext *context, int player_num, int board_id, int deck_id,
      const std::vector<std::vector<OwnedPiece*>> &pieces);
  bool CommitFormation(
      const std::vector<std::pair<std::string, Point>> &formation);
 private:
  SessionContext &context_;
  std::vector<std::vector<Tile>> board_;
  std::vector<SessionPiece*> pieces_;
  std::vector<std::vector<Card>> decks_;
  DISALLOW_COPY_AND_ASSIGN(Session);
};

NS_HIME_END

#endif  // INCLUDE_HIME_SESSION_H_

