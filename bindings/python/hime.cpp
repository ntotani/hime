#include <boost/python.hpp>
#include <string>
#include "hime/master.h"

using std::string;
using std::shared_ptr;
using boost::python::class_;
using boost::python::init;
using boost::python::return_value_policy;
using boost::python::copy_const_reference;
using boost::python::make_function;
using boost::python::register_ptr_to_python;
using hime::Skill;
using hime::Master;

BOOST_PYTHON_MODULE(hime) {
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
    .def("loadSkill", &Master::LoadSkill)
    .def("skill", &Master::skill);
  register_ptr_to_python<shared_ptr<const Skill>>();
}

