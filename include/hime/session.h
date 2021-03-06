#ifndef INCLUDE_HIME_SESSION_H_
#define INCLUDE_HIME_SESSION_H_

#include <functional>
#include <random>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <stack>
#include <memory>

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
  friend Skill;

 public:
  struct Command {
    const int piece_id;
    const Point to;
    Command(int piece_id, Point to) :piece_id(piece_id), to(to) {}
  };

  Session(std::unique_ptr<SessionContext> context, int player_num,
      int board_id, int deck_id,
      const std::vector<std::vector<std::shared_ptr<const OwnedPiece>>>
          &pieces);

  bool CommitFormation(
      const std::unordered_map<std::string, Point>& formation);
  bool CommitFormation(
      const std::unordered_map<std::string, SessionPieceState>& formation);
  std::vector<std::unique_ptr<Action>> ProcessTurn(
      const std::vector<Command>& commands);
  static std::string ActsToStr(
      const std::vector<std::unique_ptr<Action>>& acts);
  static std::vector<std::unique_ptr<Action>> StrToActs(
      const std::string& str);
  void ApplyActions(const std::vector<std::unique_ptr<Action>>& acts);
  int FindPiece(Point position) const;
  bool IsHime(int piece_id) const;
  bool IsDrop(int team_id) const;
  int CalcDamage(int actor_id, int target_id) const;
  Point RotateDir(Point dir, int team) const;

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
  bool PreAction(const std::unique_ptr<Action>& action,
      std::stack<std::unique_ptr<Action>>* resolve);
  void PostAction(const std::unique_ptr<Action>& action,
      std::stack<std::unique_ptr<Action>>* resolve);
  std::vector<Point> Card2Dirs(Card card);
  void Attack(int actor_id, int target_id,
      std::stack<std::unique_ptr<Action>>* resolve);
  void Attack(int actor_id, int target_id, int damage,
      std::stack<std::unique_ptr<Action>>* resolve);
  void Heal(int actor_id, int target_id,
      std::stack<std::unique_ptr<Action>>* resolve);
  void Heal(int actor_id, int target_id, int gain,
      std::stack<std::unique_ptr<Action>>* resolve);

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

class SessionBuilder {
 public:
  SessionBuilder(int seed, int player_num, int board_id, int deck_id);
  void PushPiece(std::shared_ptr<const OwnedPiece>, int team);
  std::unique_ptr<Session> Build();
 private:
  int seed_, player_num_, board_id_, deck_id_;
  std::vector<std::vector<std::shared_ptr<const OwnedPiece>>> pieces_;
  DISALLOW_COPY_AND_ASSIGN(SessionBuilder);
};

NS_HIME_END

#endif  // INCLUDE_HIME_SESSION_H_

