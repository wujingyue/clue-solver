cc_library(
  name = "config",
  hdrs = ["config.h"],
)

cc_library(
  name = "name_tracker",
  srcs = ["name_tracker.cc"],
  hdrs = ["name_tracker.h"],
)

cc_library(
  name = "constraint_tracker",
  srcs = ["constraint_tracker.cc"],
  hdrs = ["constraint_tracker.h"],
  deps = [":config"],
)

cc_library(
  name = "probability_computer",
  srcs = ["probability_computer.cc"],
  hdrs = ["probability_computer.h"],
  deps = [
    ":config",
    ":constraint_tracker",
  ],
)

cc_library(
  name = "solver",
  srcs = ["solver.cc"],
  hdrs = ["solver.h"],
  deps = [
    ":config",
    ":constraint_tracker",
    ":name_tracker",
    ":probability_computer",
  ],
)

cc_binary(
  name = "main",
  srcs = ["main.cc"],
  deps = [
    ":solver",
  ],
)
