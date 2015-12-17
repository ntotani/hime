#include "hime/master.h"

#include <string>
#include <vector>

using std::string;
using std::vector;
using std::make_shared;

NS_HIME_BEGIN

void Master::LoadPiece(string csv) {
  for (auto& row : Split(csv, "\n")) {
    auto cols = Split(row, ",");
    piece_[cols[0]] = make_shared<const MasterPiece>(cols[0], cols[1],
      kPlanetStr.at(cols[2]), kPieceActionStr.at(cols[3]),
      skill_[cols[4]], skill_[cols[5]],
      hime::Parameter(stoi(cols[6]), stoi(cols[7]), stoi(cols[8])));
  }
}

void Master::LoadSkill(string csv) {
  for (auto& row : Split(csv, "\n")) {
    auto cols = Split(row, ",");
    skill_[cols[0]] = make_shared<const Skill>(
      cols[0], cols[1], cols[2], stoi(cols[3]));
  }
}

vector<string> Master::Split(const string& str, const string& delim) const {
  vector<string> result;
  string::size_type pos = 0;
  while (pos != string::npos) {
    auto p = str.find(delim, pos);
    if (p == string::npos) {
      result.push_back(Trim(str.substr(pos)));
      break;
    } else {
      result.push_back(Trim(str.substr(pos, p - pos)));
    }
    pos = p + delim.size();
  }
  return move(result);
}

string Master::Trim(const string& str) const {
  auto trimCharacterList = " \t\v\r\n";
  auto left = str.find_first_not_of(trimCharacterList);
  if (left == string::npos) return "";
  auto right = str.find_last_not_of(trimCharacterList);
  return str.substr(left, right - left + 1);
}

NS_HIME_END

