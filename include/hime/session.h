#ifndef INCLUDE_HIME_SESSION_H_
#define INCLUDE_HIME_SESSION_H_

#include <functional>
#include <random>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "hime/board.h"
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
  Session(std::unique_ptr<SessionContext> context, int player_num,
      int board_id, int deck_id,
      const std::vector<std::vector<std::shared_ptr<const OwnedPiece>>>
          &pieces);
  bool CommitFormation(
      const std::unordered_map<std::string, Point> &formation);
  inline int player_num() const { return player_num_; }
  inline const std::vector<std::vector<std::shared_ptr<const OwnedPiece>>>&
      owned_pieces() const { return owned_pieces_; }
  inline const Board &board() const { return board_; }
  inline const std::vector<std::unique_ptr<SessionPiece>>& pieces() const {
      return pieces_; }
  inline const std::vector<std::vector<Card>> &decks() const { return decks_; }

 private:
  const int player_num_;
  const std::vector<std::vector<std::shared_ptr<const OwnedPiece>>>
      owned_pieces_;
  std::unique_ptr<SessionContext> context_;
  Board board_;
  std::vector<std::unique_ptr<SessionPiece>> pieces_;
  std::vector<std::vector<Card>> decks_;
  DISALLOW_COPY_AND_ASSIGN(Session);
};

NS_HIME_END

#endif  // INCLUDE_HIME_SESSION_H_

