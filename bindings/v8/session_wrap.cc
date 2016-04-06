#include "session_wrap.h"
#include "hime/master.h"

using std::make_shared;
using std::make_unique;
using std::shared_ptr;
using std::vector;
using hime::OwnedPiece;
using hime::SessionContext;
using hime::SessionContextImpl;
using hime::Master;
using hime::Session;

Nan::Persistent<v8::Function> SessionWrap::constructor;

SessionWrap::SessionWrap(std::unique_ptr<SessionContext> context, int player_num,
      int board_id, int deck_id,
      const vector<vector<shared_ptr<const OwnedPiece>>>
          &pieces) : value_(move(context), player_num, board_id, deck_id, pieces) {
}

void SessionWrap::Init(v8::Local<v8::Object> exports) {
  Nan::HandleScope scope;

  // Prepare constructor template
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Session").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  Nan::SetPrototypeMethod(tpl, "processTurn", ProcessTurn);

  constructor.Reset(tpl->GetFunction());
  exports->Set(Nan::New("Session").ToLocalChecked(), tpl->GetFunction());
}

void SessionWrap::New(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  if (info.IsConstructCall()) {
    Master master;
    master.LoadSkill("a0,dummy,dummy skill,0\n"
        "a1,全体回復,味方全員を@回復する,30\n"
        "a3,突撃,攻撃力2倍で2マス前進,0\n"
        "p0,dummy,dummy skill,0\n"
        "p1,癒やし,周りの駒が毎ターン@ずつ回復する,30\n"
        "p3,一矢,この駒を倒した相手に攻撃する,0");
    master.LoadPiece("0,dummy,mar,phys,a0,p0,40,40,40\n"
        "1,姫,sun,heal,a1,p1,60,50,80\n"
        "3,浪人,mar,phys,a3,p3,80,80,60");
    auto op1 = make_shared<const OwnedPiece>(master.piece("0"), "a");
    auto op2 = make_shared<const OwnedPiece>(master.piece("0"), "b");
    vector<vector<shared_ptr<const OwnedPiece>>> pieces = {{op1}, {op2}};
    // Invoked as constructor: `new MyObject(...)`
    // double value = info[0]->IsUndefined() ? 0 : info[0]->NumberValue();
    SessionWrap* obj = new SessionWrap(make_unique<SessionContextImpl>(0), 2, 1, 1, pieces);
    obj->Wrap(info.This());
    obj->value_.CommitFormation({{"a", {8, 3}}, {"b", {0, 3}}});
    info.GetReturnValue().Set(info.This());
  } else {
    // Invoked as plain function `MyObject(...)`, turn into construct call.
    const int argc = 1;
    v8::Local<v8::Value> argv[argc] = { info[0] };
    v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
    info.GetReturnValue().Set(cons->NewInstance(argc, argv));
  }
}

void SessionWrap::ProcessTurn(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  SessionWrap* obj = ObjectWrap::Unwrap<SessionWrap>(info.Holder());
  vector<Command> commands;
  for (auto &e : v8::Local<v8::Array>::Cast(info[0])) {
  }
  auto acts = obj->value_.ProcessTurn();
  info.GetReturnValue().Set(Nan::New(obj->value_.player_num()));
}

