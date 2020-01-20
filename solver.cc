#include <algorithm>
#include <array>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>

#include "probability_computer.h"
#include "solver.h"

using namespace std;

Solver::Solver(int max_num_data_points)
    : max_num_data_points_(max_num_data_points) {}

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
