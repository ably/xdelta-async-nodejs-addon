#include <napi.h>

#include "./XdeltaEncodeAsyncWorker.h"
#include "./XdeltaDecodeAsyncWorker.h"

using namespace Napi;

Object CreateErrorObject(Env env, int code, std::string message)
{
    Object object = Object::New(env);
    object.Set("code", code);
    object.Set("message", message);
    return object;
}

void EncodeBuffer(const CallbackInfo& info) {
    Env env = info.Env();

    if (!info[3].IsFunction()) {
        return;
    }
    Function cb = info[3].As<Function>();

    if (!info[0].IsBuffer()) {
        cb.Call({CreateErrorObject(env, 1, "Expected buffer as first argument.")});
        return;
    }
    Buffer<uint8_t> dictionary = info[0].As<Buffer<uint8_t>>();
    
    if (!info[1].IsBuffer()) {
        cb.Call({CreateErrorObject(env, 1, "Expected buffer as second argument.")});
        return;
    }
    Buffer<uint8_t> target = info[1].As<Buffer<uint8_t>>();
    
    if (!info[2].IsBuffer()) {
        cb.Call({CreateErrorObject(env, 1, "Expected buffer as third argument.")});
        return;
    }
    Buffer<uint8_t> result = info[2].As<Buffer<uint8_t>>();

    XdeltaEncodeAsyncWorker *encoder = new XdeltaEncodeAsyncWorker(dictionary, target, result, cb);
    encoder->Queue();
}

void DecodeBuffer(const CallbackInfo& info) {
    Env env = info.Env();

    if (!info[3].IsFunction()) {
        return;
    }
    Function cb = info[3].As<Function>();

    if (!info[0].IsBuffer()) {
        cb.Call({CreateErrorObject(env, 1, "Expected buffer as first argument.")});
        return;
    }
    Buffer<uint8_t> dictionary = info[0].As<Buffer<uint8_t>>();

    if (!info[1].IsBuffer()) {
        cb.Call({CreateErrorObject(env, 1, "Expected buffer as second argument.")});
        return;
    }
    Buffer<uint8_t> delta = info[1].As<Buffer<uint8_t>>();

    if (!info[2].IsBuffer()) {
        cb.Call({CreateErrorObject(env, 1, "Expected buffer as third argument.")});
        return;
    }
    Buffer<uint8_t> result = info[2].As<Buffer<uint8_t>>();

    XdeltaDecodeAsyncWorker *decoder = new XdeltaDecodeAsyncWorker(dictionary, delta, result, cb);
    decoder->Queue();
}

Object Init(Env env, Object exports) {
    exports.Set(String::New(env, "EncodeBuffer"),
                Function::New(env, EncodeBuffer));
    exports.Set(String::New(env, "DecodeBuffer"),
                Function::New(env, DecodeBuffer));
    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
