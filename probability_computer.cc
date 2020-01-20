#include "probability_computer.h"

#include <algorithm>
#include <iostream>

using namespace std;

namespace {
bool IsSuspect(int c) { return c < kSuspects; }
bool IsWeapon(int c) { return c >= kSuspects && c < kSuspects + kWeapons; }
bool IsRoom(int c) { return c >= kSuspects + kWeapons; }
}  // namespace

ProbabilityComputer::ProbabilityComputer(const ConstraintTracker& tracker,
                                         int max_num_data_points)
    : tracker_(tracker), max_num_data_points_(max_num_data_points) {}

vector<ProbabilityComputer::Guess> ProbabilityComputer::Compute() {
  num_cards_.fill(0);
  missing_suspect_ = -1;
  missing_weapon_ = -1;
  missing_room_ = -1;
  fill(&missing_probs_[0][0][0], &missing_probs_[kSuspects][0][0], 0);
  num_data_points_ = 0;

  Search(0);

  vector<Guess> guesses;
  for (int s = 0; s < kSuspects; s++) {
    for (int w = 0; w < kWeapons; w++) {
      for (int r = 0; r < kRooms; r++) {
        if (missing_probs_[s][w][r] > 0) {
          guesses.push_back(Guess{
              s, w, r, (double)missing_probs_[s][w][r] / num_data_points_});
        }
      }
    }
  }
  sort(guesses.begin(), guesses.end(),
       [](const Guess& left, const Guess& right) {
         return left.probability > right.probability;
       });
  return guesses;
}

void ProbabilityComputer::Search(int c) {
  if (c == kCards) {
    if (tracker_.CheckYes(belongs_to_)) {
      missing_probs_[missing_suspect_][missing_weapon_][missing_room_]++;
      num_data_points_++;
      if (num_data_points_ % (max_num_data_points_ / 10) == 0) {
        cerr << "Progress: " << num_data_points_ << " out of "
             << max_num_data_points_ << " data points explored." << endl;
      }
    }
    return;
  }

  if (num_data_points_ >= max_num_data_points_) {
    return;
  }

  int must_belong_to = tracker_.MustBelongTo(c);
  if (must_belong_to >= 0) {
    if (num_cards_[must_belong_to] < 3) {
      belongs_to_[c] = must_belong_to;
      num_cards_[must_belong_to]++;
      Search(c + 1);
      num_cards_[must_belong_to]--;
    }
    return;
  }

  if (IsSuspect(c) && missing_suspect_ == -1) {
    belongs_to_[c] = -1;
    missing_suspect_ = c;
    Search(c + 1);
    missing_suspect_ = -1;
  }

  if (IsWeapon(c) && missing_weapon_ == -1) {
    belongs_to_[c] = -1;
    missing_weapon_ = c - kSuspects;
    Search(c + 1);
    missing_weapon_ = -1;
  }

  if (IsRoom(c) && missing_room_ == -1) {
    belongs_to_[c] = -1;
    missing_room_ = c - kSuspects - kWeapons;
    Search(c + 1);
    missing_room_ = -1;
  }

  for (int p = 1; p < kPlayers; p++) {
    if (tracker_.No(c, p)) {
      continue;
    }
    if (num_cards_[p] >= 3) {
      continue;
    }
    belongs_to_[c] = p;
    num_cards_[p]++;
    Search(c + 1);
    num_cards_[p]--;
  }
}
