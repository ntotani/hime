#ifndef INCLUDE_HIME_SESSION_H_
#define INCLUDE_HIME_SESSION_H_

#include <functional>
#include <random>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "hime/action.h"
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
  struct Command {
    const int piece_id;
    const int card_idx;
    Command(int piece_id, int card_idx)
        :piece_id(piece_id), card_idx(card_idx) {}
  };

  Session(std::unique_ptr<SessionContext> context, int player_num,
      int board_id, int deck_id,
      const std::vector<std::vector<std::shared_ptr<const OwnedPiece>>>
          &pieces);

  bool CommitFormation(
      const std::unordered_map<std::string, Point>& formation);
  std::vector<std::unique_ptr<Action>> ProcessTurn(
      const std::vector<Command>& commands);
  int FindPiece(Point position) const;

  inline int player_num() const { return player_num_; }
  inline const std::vector<std::vector<std::shared_ptr<const OwnedPiece>>>&
      owned_pieces() const { return owned_pieces_; }
  inline const Board &board() const { return board_; }
  inline const std::vector<std::unique_ptr<SessionPiece>>& pieces() const {
      return pieces_; }
  inline const std::vector<std::vector<Card>>& decks() const { return decks_; }
  inline const std::vector<std::vector<Card>>& hands() const { return hands_; }
  inline const std::vector<std::vector<Card>>& trash() const { return trash_; }

 private:
  void DrawCard();
  std::vector<std::unique_ptr<Action>> ApplyDir(int piece_id, Point dir);
  std::vector<std::unique_ptr<Action>> TryMove(int piece_id, Point position);
  std::vector<std::unique_ptr<Action>> CommitMove(int piece_id, Point position);
  std::vector<Point> Card2Dirs(Card card);

  const int player_num_;
  const std::vector<std::vector<std::shared_ptr<const OwnedPiece>>>
      owned_pieces_;
  std::unique_ptr<SessionContext> context_;
  Board board_;
  std::vector<std::unique_ptr<SessionPiece>> pieces_;
  std::vector<std::vector<Card>> decks_;
  std::vector<std::vector<Card>> hands_;
  std::vector<std::vector<Card>> trash_;
  std::vector<int> hand_size_;
  DISALLOW_COPY_AND_ASSIGN(Session);
};

NS_HIME_END

#endif  // INCLUDE_HIME_SESSION_H_

