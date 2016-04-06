#include <nan.h>
#include "session_wrap.h"

void InitAll(v8::Local<v8::Object> exports) {
  SessionWrap::Init(exports);
}

NODE_MODULE(hime, InitAll)

