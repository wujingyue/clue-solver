#include "constraint_tracker.h"

#include <algorithm>

using namespace std;

ConstraintTracker::ConstraintTracker() {
  fill(&no_[0][0], &no_[kCards][0], 0);
  must_belong_to_.fill(-1);
}

void ConstraintTracker::AddNo(int card, int player) {
  no_[card][player] = true;
}

void ConstraintTracker::AddNo(int suspect, int weapon, int room, int player) {
  no_[suspect][player] = true;
  no_[kSuspects + weapon][player] = true;
  no_[kSuspects + kWeapons + room][player] = true;
}

void ConstraintTracker::AddYes(int suspect, int weapon, int room, int player) {
  yes_.push_back(make_tuple(suspect, kSuspects + weapon,
                            kSuspects + kWeapons + room, player));
}

bool ConstraintTracker::No(int card, int player) const {
  return no_[card][player];
}

bool ConstraintTracker::CheckYes(const array<int, kCards>& belongs_to) const {
  for (const auto& entry : yes_) {
    int c1, c2, c3, p;
    tie(c1, c2, c3, p) = entry;
    if (belongs_to[c1] != p && belongs_to[c2] != p && belongs_to[c3] != p) {
      return false;
    }
  }
  return true;
}

void ConstraintTracker::SetMustBelongTo(int card, int player) {
  must_belong_to_[card] = player;
}

int ConstraintTracker::MustBelongTo(int card) const {
  return must_belong_to_[card];
}
