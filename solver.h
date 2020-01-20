#ifndef SOLVER_H
#define SOLVER_H

#include <vector>

#include "constraint_tracker.h"
#include "name_tracker.h"

class Solver {
 public:
  struct Guess {
    std::string suspect;
    std::string weapon;
    std::string room;
    double probability;
  };

  explicit Solver(int max_num_data_points);
  bool ComputeProbabilities(const std::vector<std::string>& lines,
                            std::vector<Guess>* guesses);

 private:
  bool Parse(const std::vector<std::string>& lines,
             ConstraintTracker* constraint_tracker);

  const NameTracker name_tracker_;
  const int max_num_data_points_;
};

#endif  // SOLVER_H
