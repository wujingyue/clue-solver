#include "name_tracker.h"

#include <algorithm>
#include <iostream>

using namespace std;

NameTracker::NameTracker()
    : suspect_names_(
          {"mustard", "orchid", "green", "peacock", "scarlett", "peach"}),
      weapon_names_(
          {"candlestick", "dagger", "lead", "revolver", "rope", "wrench"}),
      room_names_({"ballroom", "billiard", "conservatory", "dining", "hall",
                   "kitchen", "library", "lounge", "study"}) {
  copy(suspect_names_.begin(), suspect_names_.end(),
       back_inserter(card_names_));
  copy(weapon_names_.begin(), weapon_names_.end(), back_inserter(card_names_));
  copy(room_names_.begin(), room_names_.end(), back_inserter(card_names_));
}

int NameTracker::SuspectId(const string& name) const {
  auto i = find(suspect_names_.begin(), suspect_names_.end(), name);
  if (i == suspect_names_.end()) {
    cerr << "Invalid suspect name: " << name << endl;
    exit(1);
  }
  return i - suspect_names_.begin();
}

int NameTracker::WeaponId(const string& name) const {
  auto i = find(weapon_names_.begin(), weapon_names_.end(), name);
  if (i == weapon_names_.end()) {
    cerr << "Invalid weapon name: " << name << endl;
    exit(1);
  }
  return i - weapon_names_.begin();
}

int NameTracker::RoomId(const string& name) const {
  auto i = find(room_names_.begin(), room_names_.end(), name);
  if (i == room_names_.end()) {
    cerr << "Invalid room name: " << name << endl;
    exit(1);
  }
  return i - room_names_.begin();
}

int NameTracker::CardId(const string& name) const {
  auto i = find(card_names_.begin(), card_names_.end(), name);
  if (i == card_names_.end()) {
    cerr << "Invalid card name: " << name << endl;
    exit(1);
  }
  return i - card_names_.begin();
}

string NameTracker::SuspectName(int s) const { return suspect_names_[s]; }
string NameTracker::WeaponName(int w) const { return weapon_names_[w]; }
string NameTracker::RoomName(int r) const { return room_names_[r]; }
