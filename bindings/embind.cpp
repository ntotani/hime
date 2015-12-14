#include <string>
#include <vector>

#include "emscripten/bind.h"

#include "hime/session.h"

namespace {

using std::make_shared;
using std::make_unique;
using std::string;
using std::shared_ptr;
using std::unique_ptr;
using std::vector;
using emscripten::base;
using emscripten::class_;
using emscripten::enum_;
using emscripten::function;
using emscripten::pure_virtual;
using emscripten::wrapper;
using emscripten::register_vector;
using hime::MasterPiece;
using hime::OwnedPiece;
using hime::SessionPiece;
using hime::Parameter;
using hime::Planet;
using hime::Session;
using hime::SessionContext;
using hime::SessionContextImpl;
using hime::Skill;

EMSCRIPTEN_BINDINGS(hime) {
  enum_<Planet>("Planet")
      .value("kSun", Planet::kSun)
      .value("kMoon", Planet::kMoon)
      .value("kMars", Planet::kMars)
      .value("kMercury", Planet::kMercury)
      .value("kJupiter", Planet::kJupiter)
      .value("kVenus", Planet::kVenus)
      .value("kSaturn", Planet::kSaturn);
  class_<Skill>("Skill")
      .smart_ptr_constructor("shared_ptr<const Skill>",
          &make_shared<const Skill,
          const string&, const string&, const string&, int>)
      .property("id", &Skill::id)
      .property("name", &Skill::name)
      .property("desc", &Skill::desc)
      .property("rate", &Skill::rate);
}

EMSCRIPTEN_BINDINGS(piece) {
  class_<Parameter>("Parameter")
      .constructor<int, int, int>()
      .property("power", &Parameter::power)
      .property("defense", &Parameter::defense)
      .property("resist", &Parameter::resist);
  class_<MasterPiece>("MasterPiece")
      .smart_ptr_constructor("shared_ptr<const MasterPiece>",
          &make_shared<const MasterPiece, const string&, const string&, Planet,
          shared_ptr<const Skill>, shared_ptr<const Skill>, Parameter>)
      .property("id", &MasterPiece::id)
      .property("name", &MasterPiece::name)
      .property("planet", &MasterPiece::planet)
      .property("active_skill", &MasterPiece::active_skill)
      .property("passive_skill", &MasterPiece::passive_skill)
      .property("param", &MasterPiece::param);
  class_<OwnedPiece>("OwnedPiece")
      .smart_ptr_constructor("shared_ptr<const OwnedPiece>",
          &make_shared<const OwnedPiece,
          shared_ptr<const MasterPiece>, const string&>)
      .property("master", &OwnedPiece::master)
      .property("id", &OwnedPiece::id);
  class_<SessionPiece>("SessionPiece")
      .property("id", &SessionPiece::id);
}

struct SessionContextWrapper : public wrapper<SessionContext> {
  EMSCRIPTEN_WRAPPER(SessionContextWrapper);
  int random() {
    return call<int>("random");
  }
};

unique_ptr<Session> session_factory(int seed, int player_num,
    int board_id, int deck_id,
    const vector<vector<shared_ptr<const OwnedPiece>>> &pieces) {
  return make_unique<Session>(make_unique<SessionContextImpl>(seed),
      player_num, board_id, deck_id, pieces);
}

EMSCRIPTEN_BINDINGS(session) {
  class_<SessionContext>("SessionContext")
      .function("random", &SessionContext::random, pure_virtual())
      .allow_subclass<SessionContextWrapper>("SessionContextWrapper");
  class_<SessionContextImpl, base<SessionContext>>("SessionContextImpl")
      .function("random", &SessionContextImpl::random);
  register_vector<shared_ptr<const OwnedPiece>>("OwnedPieceVector");
  register_vector<vector<shared_ptr<const OwnedPiece>>>(
      "OwnedPieceVectorVector");
  class_<Session>("Session")
      .property("player_num", &Session::player_num)
      .property("owned_pieces", &Session::owned_pieces);
  function("session_factory", &session_factory);
}

}  // namespace

