#include <string>

#include "emscripten/bind.h"

#include "hime/session.h"

using std::make_shared;
using std::string;
using std::shared_ptr;
using std::unique_ptr;
using std::vector;
using emscripten::base;
using emscripten::class_;
using emscripten::enum_;
using emscripten::pure_virtual;
using emscripten::wrapper;
using hime::MasterPiece;
using hime::Parameter;
using hime::Planet;
using hime::Session;
using hime::SessionContext;
using hime::SessionContextImpl;
using hime::Skill;

struct SessionContextWrapper : public wrapper<SessionContext> {
    EMSCRIPTEN_WRAPPER(SessionContextWrapper);
    int random() {
        return call<int>("random");
    }
};

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
  class_<Parameter>("Parameter")
      .constructor<int, int, int>()
      .property("power", &Parameter::power)
      .property("defense", &Parameter::defense)
      .property("resist", &Parameter::resist);
  class_<MasterPiece>("MasterPiece")
      .constructor<const string&, const string&, Planet,
          shared_ptr<const Skill>, shared_ptr<const Skill>, Parameter>()
      .property("id", &MasterPiece::id)
      .property("name", &MasterPiece::name)
      .property("planet", &MasterPiece::planet)
      .property("active_skill", &MasterPiece::active_skill)
      .property("passive_skill", &MasterPiece::passive_skill)
      .property("param", &MasterPiece::param);
  class_<SessionContext>("SessionContext")
      .function("random", &SessionContext::random, pure_virtual())
      .allow_subclass<SessionContextWrapper>("SessionContextWrapper");
  class_<SessionContextImpl, base<SessionContext>>("SessionContextImpl")
      .constructor<int>()
      .function("random", &SessionContextImpl::random);
  /*
  class_<Session>("Session")
      .constructor<unique_ptr<SessionContext>, int, int, int, const vector<vector<shared_ptr<const OwnedPiece>>>&>();
    */
}

