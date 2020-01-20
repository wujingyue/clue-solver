#ifndef NAME_TRACKER_H
#define NAME_TRACKER_H

#include <string>
#include <vector>

class NameTracker {
 public:
  NameTracker();
  int SuspectId(const std::string& name) const;
  int WeaponId(const std::string& name) const;
  int RoomId(const std::string& name) const;
  int CardId(const std::string& name) const;
  std::string SuspectName(int s) const;
  std::string WeaponName(int w) const;
  std::string RoomName(int r) const;

 private:
  std::vector<std::string> suspect_names_;
  std::vector<std::string> weapon_names_;
  std::vector<std::string> room_names_;
  std::vector<std::string> card_names_;
};

#endif  // NAME_TRACKER_H
