#ifndef BINDINGS_V8_SESSION_WRAP_H_
#define BINDINGS_V8_SESSION_WRAP_H_

#include <nan.h>
#include <vector>
#include "hime/session.h"

class SessionWrap : public Nan::ObjectWrap {
 public:
  static void Init(v8::Local<v8::Object> exports);

 private:
  SessionWrap(std::unique_ptr<hime::SessionContext> context, int player_num,
      int board_id, int deck_id,
      const std::vector<std::vector<std::shared_ptr<const hime::OwnedPiece>>>
          &pieces);

  static void New(const Nan::FunctionCallbackInfo<v8::Value>& info);
  static void GetPlayerNum(const Nan::FunctionCallbackInfo<v8::Value>& info);
  static void ProcessTurn(const Nan::FunctionCallbackInfo<v8::Value>& info);
  static Nan::Persistent<v8::Function> constructor;
  hime::Session value_;
};

#endif  // BINDINGS_V8_SESSION_WRAP_H_

