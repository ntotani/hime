#include <boost/python.hpp>
#include <string>
#include "hime/piece.h"
#include "hime/master.h"

using std::string;
using std::shared_ptr;
using boost::python::class_;
using boost::python::enum_;
using boost::python::init;
using boost::python::return_value_policy;
using boost::python::copy_const_reference;
using boost::python::make_function;
using boost::python::register_ptr_to_python;
using hime::Parameter;
using hime::MasterPiece;
using hime::Skill;
using hime::Master;
using hime::Planet;
using hime::PieceAction;

BOOST_PYTHON_MODULE(hime) {
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

  // piece.h
  class_<Parameter>("Parameter", init<int>()).
    def(init<int, int, int>())
    .add_property("power", &Parameter::power)
    .add_property("defense", &Parameter::defense)
    .add_property("resist", &Parameter::resist);
  class_<MasterPiece, boost::noncopyable>("MasterPiece", init<
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

  class_<Skill, boost::noncopyable>(
      "Skill", init<string, string, string, int>())
    .add_property("id", make_function(
          &Skill::id, return_value_policy<copy_const_reference>()))
    .add_property("name", make_function(
          &Skill::name, return_value_policy<copy_const_reference>()))
    .add_property("desc", make_function(
          &Skill::desc, return_value_policy<copy_const_reference>()))
    .add_property("rate", &Skill::rate);
  class_<Master, boost::noncopyable>("Master")
    .def("load_piece", &Master::LoadPiece)
    .def("load_skill", &Master::LoadSkill)
    .def("piece", &Master::piece)
    .def("skill", &Master::skill);
  register_ptr_to_python<shared_ptr<const MasterPiece>>();
  register_ptr_to_python<shared_ptr<const Skill>>();
}

