#include <boost/python.hpp>
#include "hime/master.h"

using boost::python::class_;
using hime::Master;

BOOST_PYTHON_MODULE(hime) {
  class_<Master, boost::noncopyable>("Master")
    .def("loadSkill", &Master::LoadSkill);
}

