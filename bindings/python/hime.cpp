#include <boost/python.hpp>
#include <string>
#include <vector>
#include "hime/master.h"
#include "hime/piece.h"
#include "hime/session.h"

using std::string;
using std::vector;
using std::shared_ptr;
using std::unique_ptr;
using std::make_shared;
using boost::noncopyable;
using boost::python::def;
using boost::python::class_;
using boost::python::enum_;
using boost::python::init;
using boost::python::no_init;
using boost::python::return_value_policy;
using boost::python::copy_const_reference;
using boost::python::make_function;
using boost::python::register_ptr_to_python;
using boost::python::manage_new_object;
using hime::Parameter;
using hime::MasterPiece;
using hime::OwnedPiece;
using hime::Skill;
using hime::Master;
using hime::Planet;
using hime::PieceAction;
using hime::Session;
using hime::SessionBuilder;

template <typename T, typename C, typename ...Args>
boost::python::object adapt_unique(unique_ptr<T> (C::*fn)(Args...)) {
  return make_function(
      [fn](C& self, Args... args) { return (self.*fn)(args...).release(); },
      return_value_policy<manage_new_object>(),
      boost::mpl::vector<T*, C&, Args...>());
}

shared_ptr<const OwnedPiece> create_owned_piece(
    shared_ptr<const MasterPiece> master, const string& id) {
  return make_shared<const OwnedPiece>(master, id);
}

BOOST_PYTHON_MODULE(hime) {
  // hime.h
  enum_<Planet>("Planet")
    .value("sun", Planet::kSun)
    .value("moon", Planet::kMoon)
    .value("mars", Planet::kMars)
    .value("mercury", Planet::kMercury)
    .value("jupiter", Planet::kJupiter)
    .value("venus", Planet::kVenus)
    .value("saturn", Planet::kSaturn);
  enum_<PieceAction>("PieceAction")
    .value("physical", PieceAction::kPhysical)
    .value("magical", PieceAction::kMagical)
    .value("heal", PieceAction::kHeal);
  class_<Skill, noncopyable>(
      "Skill", init<string, string, string, int>())
    .add_property("id", make_function(
          &Skill::id, return_value_policy<copy_const_reference>()))
    .add_property("name", make_function(
          &Skill::name, return_value_policy<copy_const_reference>()))
    .add_property("desc", make_function(
          &Skill::desc, return_value_policy<copy_const_reference>()))
    .add_property("rate", &Skill::rate);

  // master.h
  class_<Master, noncopyable>("Master")
    .def("load_piece", &Master::LoadPiece)
    .def("load_skill", &Master::LoadSkill)
    .def("piece", &Master::piece)
    .def("skill", &Master::skill);

  // piece.h
  class_<Parameter>("Parameter", init<int>()).
    def(init<int, int, int>())
    .add_property("power", &Parameter::power)
    .add_property("defense", &Parameter::defense)
    .add_property("resist", &Parameter::resist);
  class_<MasterPiece, noncopyable>("MasterPiece", init<
      const string&, const string&, Planet, PieceAction,
      shared_ptr<const Skill>, shared_ptr<const Skill>, Parameter>())
    .add_property("id", make_function(
          &MasterPiece::id, return_value_policy<copy_const_reference>()))
    .add_property("name", make_function(
          &MasterPiece::name, return_value_policy<copy_const_reference>()))
    .add_property("planet", &MasterPiece::planet)
    .add_property("action", &MasterPiece::action)
    .add_property("active_skill", &MasterPiece::active_skill)
    .add_property("passive_skill", &MasterPiece::passive_skill)
    .add_property("param", &MasterPiece::param);
  class_<OwnedPiece, noncopyable>("OwnedPiece",
      init<shared_ptr<const MasterPiece>, const string&>())
    .add_property("master", &OwnedPiece::master)
    .add_property("id", make_function(
          &OwnedPiece::id, return_value_policy<copy_const_reference>()));
  def("create_owned_piece", &create_owned_piece);

  // session.h
  class_<SessionBuilder, noncopyable>("SessionBuilder",
      init<int, int, int, int>())
    .def("push_piece", &SessionBuilder::PushPiece)
    .def("build", adapt_unique(&SessionBuilder::Build));
  class_<Session, noncopyable>("Session", no_init)
    .add_property("player_num", &Session::player_num);

  // types
  register_ptr_to_python<shared_ptr<const Skill>>();
  register_ptr_to_python<shared_ptr<const MasterPiece>>();
  register_ptr_to_python<shared_ptr<const OwnedPiece>>();
}

