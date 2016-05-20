#include "hime/master.h"

#include <string>
#include <vector>
#include <unordered_map>

using std::string;
using std::vector;
using std::unordered_map;
using std::make_shared;

NS_HIME_BEGIN

const unordered_map<char, Point> kDirCodes = {
  {'a', {-2, 0}}, {'b', {-1,  1}}, {'c', {-1, -1}},
  {'d', { 1, 1}}, {'e', { 1, -1}}, {'f', { 2,  0}}
};

void Master::LoadPiece(string csv) {
  for (auto& row : Split(csv, "\n")) {
    auto cols = Split(row, ",");
    auto id      = cols[0];
    auto name    = cols[1];
    auto planet  = cols[2];
    auto action  = cols[3];
    auto skill   = cols[4];
    auto power   = stoi(cols[5]);
    auto defense = stoi(cols[6]);
    auto resist  = stoi(cols[7]);
    vector<Point> range;
    for (auto e : cols[8]) {
      range.push_back(kDirCodes.at(e));
    }
    piece_[id] = make_shared<const MasterPiece>(id, name,
      kPlanetStr.at(planet), kPieceActionStr.at(action), skill_[skill],
      hime::Parameter(power, defense, resist), range);
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

