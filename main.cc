#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "solver.h"

using namespace std;

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
