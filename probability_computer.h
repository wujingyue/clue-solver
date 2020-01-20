#ifndef PROBABILITY_COMPUTER_H
#define PROBABILITY_COMPUTER_H

#include <array>
#include <vector>

#include "config.h"
#include "constraint_tracker.h"

class ProbabilityComputer {
 public:
  struct Guess {
    int suspect;
    int weapon;
    int room;
    double probability;
  };

  ProbabilityComputer(const ConstraintTracker& tracker,
                      int max_num_data_points);
  std::vector<Guess> Compute();

 private:
  void Search(int c);

  const ConstraintTracker& tracker_;
  const int max_num_data_points_;

  std::array<int, kCards> belongs_to_;
  std::array<int, kPlayers> num_cards_;
  int missing_suspect_ = -1;
  int missing_weapon_ = -1;
  int missing_room_ = -1;
  int missing_probs_[kSuspects][kWeapons][kRooms];
  int num_data_points_ = 0;
};

#endif  // PROBABILITY_COMPUTER_H
