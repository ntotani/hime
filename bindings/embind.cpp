#include <emscripten/bind.h>

#include "hime/session.h"

using emscripten::base;
using emscripten::class_;
using emscripten::pure_virtual;
using emscripten::wrapper;
using hime::Session;
using hime::SessionContext;
using hime::SessionContextImpl;

struct SessionContextWrapper : public wrapper<SessionContext> {
    EMSCRIPTEN_WRAPPER(SessionContextWrapper);
    int random() {
        return call<int>("random");
    }
};

EMSCRIPTEN_BINDINGS(hime) {
  class_<SessionContext>("SessionContext")
    .function("random", &SessionContext::random, pure_virtual())
    .allow_subclass<SessionContextWrapper>("SessionContextWrapper");
  class_<SessionContextImpl, base<SessionContext>>("SessionContextImpl")
    .constructor<int>()
    .function("random", &SessionContextImpl::random);
  class_<Session>("Session")
    .constructor<int, SessionContext&>()
    .function("applyAction", &Session::applyAction)
    .function("getState", &Session::getState);
}

