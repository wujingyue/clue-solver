#include <algorithm>
#include <array>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "config.h"

using namespace std;

class NameTracker {
 public:
  NameTracker() {
    copy(suspect_names_.begin(), suspect_names_.end(),
         back_inserter(card_names_));
    copy(weapon_names_.begin(), weapon_names_.end(),
         back_inserter(card_names_));
    copy(room_names_.begin(), room_names_.end(), back_inserter(card_names_));
  }

  int SuspectId(const string& name) {
    auto i = find(suspect_names_.begin(), suspect_names_.end(), name);
    if (i == suspect_names_.end()) {
      cerr << "Invalid suspect name: " << name << endl;
      exit(1);
    }
    return i - suspect_names_.begin();
  }

  int WeaponId(const string& name) {
    auto i = find(weapon_names_.begin(), weapon_names_.end(), name);
    if (i == weapon_names_.end()) {
      cerr << "Invalid weapon name: " << name << endl;
      exit(1);
    }
    return i - weapon_names_.begin();
  }

  int RoomId(const string& name) {
    auto i = find(room_names_.begin(), room_names_.end(), name);
    if (i == room_names_.end()) {
      cerr << "Invalid room name: " << name << endl;
      exit(1);
    }
    return i - room_names_.begin();
  }

  int CardId(const string& name) {
    auto i = find(card_names_.begin(), card_names_.end(), name);
    if (i == card_names_.end()) {
      cerr << "Invalid card name: " << name << endl;
      exit(1);
    }
    return i - card_names_.begin();
  }

  string SuspectName(int s) const { return suspect_names_[s]; }
  string WeaponName(int w) const { return weapon_names_[w]; }
  string RoomName(int r) const { return room_names_[r]; }

 private:
  vector<string> suspect_names_ = {"mustard", "orchid",   "green",
                                   "peacock", "scarlett", "peach"};
  vector<string> weapon_names_ = {"candlestick", "dagger", "lead",
                                  "revolver",    "rope",   "wrench"};
  vector<string> room_names_ = {"ballroom", "billiard", "conservatory",
                                "dining",   "hall",     "kitchen",
                                "library",  "lounge",   "study"};
  vector<string> card_names_;
};

class ConstraintTracker {
 public:
  ConstraintTracker() {
    fill(&no_[0][0], &no_[kCards][0], 0);
    must_belong_to_.fill(-1);
  }

  void AddNo(int card, int player) { no_[card][player] = true; }

  void AddNo(int suspect, int weapon, int room, int player) {
    no_[suspect][player] = true;
    no_[kSuspects + weapon][player] = true;
    no_[kSuspects + kWeapons + room][player] = true;
  }

  void AddYes(int suspect, int weapon, int room, int player) {
    yes_.push_back(make_tuple(suspect, kSuspects + weapon,
                              kSuspects + kWeapons + room, player));
  }

  bool No(int card, int player) const { return no_[card][player]; }

  bool CheckYes(const array<int, kCards>& belongs_to) const {
    for (const auto& entry : yes_) {
      int c1, c2, c3, p;
      tie(c1, c2, c3, p) = entry;
      if (belongs_to[c1] != p && belongs_to[c2] != p && belongs_to[c3] != p) {
        return false;
      }
    }
    return true;
  }

  void SetMustBelongTo(int card, int player) { must_belong_to_[card] = player; }

  int MustBelongTo(int card) const { return must_belong_to_[card]; }

 private:
  bool no_[kCards][kPlayers];
  array<int, kCards> must_belong_to_;
  vector<tuple<int, int, int, int>> yes_;
};

class ProbabilityComputer {
 public:
  struct Guess {
    int suspect;
    int weapon;
    int room;
    double probability;
  };

  ProbabilityComputer(const ConstraintTracker& tracker, int max_num_data_points)
      : tracker_(tracker), max_num_data_points_(max_num_data_points) {
    fill(&num_cards_[0], &num_cards_[kPlayers], 0);
    fill(&missing_probs_[0][0][0], &missing_probs_[kSuspects][0][0], 0);
  }

  vector<Guess> Compute() {
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

 private:
  static bool IsSuspect(int c) { return c < kSuspects; }

  static bool IsWeapon(int c) {
    return c >= kSuspects && c < kSuspects + kWeapons;
  }

  static bool IsRoom(int c) { return c >= kSuspects + kWeapons; }

  void Search(int c) {
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

  const ConstraintTracker& tracker_;
  const int max_num_data_points_;
  array<int, kCards> belongs_to_;
  int num_cards_[kPlayers];
  int missing_suspect_ = -1;
  int missing_weapon_ = -1;
  int missing_room_ = -1;
  int missing_probs_[kSuspects][kWeapons][kRooms];
  int num_data_points_ = 0;
};

class Solver {
 public:
  struct Guess {
    string suspect;
    string weapon;
    string room;
    double probability;
  };

  Solver(int max_num_data_points) : max_num_data_points_(max_num_data_points) {}
  bool ComputeProbabilities(const vector<string>& lines,
                            vector<Guess>* guesses);

 private:
  bool Parse(const vector<string>& lines,
             ConstraintTracker* constraint_tracker);

  NameTracker name_tracker_;
  const int max_num_data_points_;
};

bool Solver::ComputeProbabilities(const vector<string>& lines,
                                  vector<Guess>* guesses) {
  ConstraintTracker constraint_tracker;
  if (!Parse(lines, &constraint_tracker)) {
    return false;
  }

  ProbabilityComputer computer(constraint_tracker, max_num_data_points_);
  vector<ProbabilityComputer::Guess> computer_guesses = computer.Compute();

  guesses->clear();
  transform(computer_guesses.begin(), computer_guesses.end(),
            back_inserter(*guesses),
            [this](const ProbabilityComputer::Guess& computer_guess) {
              return Guess{name_tracker_.SuspectName(computer_guess.suspect),
                           name_tracker_.WeaponName(computer_guess.weapon),
                           name_tracker_.RoomName(computer_guess.room),
                           computer_guess.probability};
            });
  return true;
}

bool Solver::Parse(const vector<string>& lines,
                   ConstraintTracker* constraint_tracker) {
  for (const string& line : lines) {
    istringstream iss(line);
    string command;
    iss >> command;
    if (command == "no") {
      string suspect_name, weapon_name, room_name;
      iss >> suspect_name >> weapon_name >> room_name;
      int suspect = name_tracker_.SuspectId(suspect_name);
      int weapon = name_tracker_.WeaponId(weapon_name);
      int room = name_tracker_.RoomId(room_name);
      int player;
      while (iss >> player) {
        constraint_tracker->AddNo(suspect, weapon, room, player);
      }
    } else if (command == "noyes" || command == "yes") {
      string suspect_name, weapon_name, room_name;
      iss >> suspect_name >> weapon_name >> room_name;
      int suspect = name_tracker_.SuspectId(suspect_name);
      int weapon = name_tracker_.WeaponId(weapon_name);
      int room = name_tracker_.RoomId(room_name);

      vector<int> players;
      int player;
      while (iss >> player) {
        players.push_back(player);
      }
      for (size_t i = 0; i + 1 < players.size(); i++) {
        constraint_tracker->AddNo(suspect, weapon, room, players[i]);
      }
      constraint_tracker->AddYes(suspect, weapon, room, players.back());
    } else if (command == "must") {
      string card_name;
      int player;
      iss >> card_name >> player;
      constraint_tracker->SetMustBelongTo(name_tracker_.CardId(card_name),
                                          player);
    } else {
      cerr << "Unknown command: " << command << endl;
      return false;
    }
  }
  return true;
}

int main(int argc, char* argv[]) {
  int max_num_data_points = 10000000;
  if (argc > 1) {
    max_num_data_points = atoi(argv[1]);
  }

  vector<string> lines;
  string line;
  while (getline(cin, line)) {
    lines.push_back(line);
  }

  Solver solver(max_num_data_points);
  vector<Solver::Guess> guesses;
  if (!solver.ComputeProbabilities(lines, &guesses)) {
    cerr << "Failed to compute probabilities" << endl;
    exit(1);
  }

  cout << "Top guesses:" << endl;
  int printed = 0;
  for (const auto& guess : guesses) {
    cout << guess.suspect << ' ' << guess.weapon << ' ' << guess.room << ' '
         << guess.probability << endl;
    printed++;
    if (printed >= 10) {
      break;
    }
  }

  cout << endl << "Top rooms:" << endl;
  unordered_map<string, double> room_prob;
  for (const auto& guess : guesses) {
    room_prob[guess.room] += guess.probability;
  }
  vector<pair<string, double>> sorted_room_probs;
  copy(room_prob.begin(), room_prob.end(), back_inserter(sorted_room_probs));
  sort(sorted_room_probs.begin(), sorted_room_probs.end(),
       [](const pair<string, double>& left, const pair<string, double>& right) {
         return left.second > right.second;
       });
  for (const auto& kv : sorted_room_probs) {
    cout << kv.first << ' ' << kv.second << endl;
  }
  return 0;
}
