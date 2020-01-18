CXXFLAGS = -std=gnu++17 -g -O2 -Wall -Werror

clue_solver: clue_solver.cc
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f clue_solver

.PHONY: clean
