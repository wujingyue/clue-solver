#ifndef CONSTRAINT_TRACKER_H
#define CONSTRAINT_TRACKER_H

#include <array>
#include <tuple>
#include <vector>

#include "config.h"

class ConstraintTracker {
 public:
  ConstraintTracker();

  void AddNo(int card, int player);
  void AddNo(int suspect, int weapon, int room, int player);
  bool No(int card, int player) const;

  void AddYes(int suspect, int weapon, int room, int player);
  bool CheckYes(const std::array<int, kCards>& belongs_to) const;

  void SetMustBelongTo(int card, int player);
  int MustBelongTo(int card) const;

 private:
  bool no_[kCards][kPlayers];
  std::array<int, kCards> must_belong_to_;
  std::vector<std::tuple<int, int, int, int>> yes_;
};

#endif  // CONSTRAINT_TRACKER_H
