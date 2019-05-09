#include <napi.h>

#include "./XdeltaEncodeAsyncWorker.h"
#include "./XdeltaDecodeAsyncWorker.h"

using namespace std;
using namespace Napi;

Object CreateErrorObject(Env env, int code, std::string message)
{
    Object object = Object::New(env);
    object.Set("code", code);
    object.Set("message", message);
    return object;
}

Value EncodeBuffer(const CallbackInfo& info) {
    Env env = info.Env();

    if (info.Length() != 5 || !info[4].IsFunction()) {
        return env.Undefined();
    }

    Function callback = info[4].As<Function>();

    if (!info[0].IsBuffer()) {
        callback.Call({CreateErrorObject(env, 1, "Expected buffer as first argument.")});
        return env.Undefined();
    }
    Buffer<uint8_t> dictionary = info[0].As<Buffer<uint8_t>>();
    
    if (!info[1].IsBuffer()) {
        callback.Call({CreateErrorObject(env, 1, "Expected buffer as second argument.")});
        return env.Undefined();
    }
    Buffer<uint8_t> target = info[1].As<Buffer<uint8_t>>();
    
    if (!info[2].IsBuffer()) {
        callback.Call({CreateErrorObject(env, 1, "Expected buffer as third argument.")});
        return env.Undefined();
    }
    Buffer<uint8_t> result = info[2].As<Buffer<uint8_t>>();

    if (!info[3].IsNumber()) {
        callback.Call({CreateErrorObject(env, 1, "Expected number as fourth argument.")});
        return env.Undefined();
    }
    int32_t stringMatcherType = info[3].As<Number>().Int32Value();

    if(stringMatcherType < XD3_SMATCH_DEFAULT ||
    stringMatcherType > XD3_SMATCH_SOFT)
    {
        callback.Call({CreateErrorObject(env, 1, "Illegal fourth argument value. Only values defined in xd3SmatchConfig allowed.")});
        return env.Undefined();        
    }

    XdeltaEncodeAsyncWorker *encoder = NULL;
    string error = XdeltaEncodeAsyncWorker::New(dictionary, target, result, stringMatcherType, callback, &encoder);
    if (!error.empty()) {
        callback.Call({CreateErrorObject(env, 2, error)});
        return env.Undefined();
    }

    encoder->Queue();
    return Number::New(env, encoder->id);
}

void DecodeBuffer(const CallbackInfo& info) {
    Env env = info.Env();

    if (!info[3].IsFunction()) {
        return;
    }
    Function callback = info[3].As<Function>();

    if (!info[0].IsBuffer()) {
        callback.Call({CreateErrorObject(env, 1, "Expected buffer as first argument.")});
        return;
    }
    Buffer<uint8_t> dictionary = info[0].As<Buffer<uint8_t>>();

    if (!info[1].IsBuffer()) {
        callback.Call({CreateErrorObject(env, 1, "Expected buffer as second argument.")});
        return;
    }
    Buffer<uint8_t> delta = info[1].As<Buffer<uint8_t>>();

    if (!info[2].IsBuffer()) {
        callback.Call({CreateErrorObject(env, 1, "Expected buffer as third argument.")});
        return;
    }
    Buffer<uint8_t> result = info[2].As<Buffer<uint8_t>>();

    XdeltaDecodeAsyncWorker *decoder = new XdeltaDecodeAsyncWorker(dictionary, delta, result, callback);
    decoder->Queue();
}

Value CancelEncode(const CallbackInfo& info) {
    Env env = info.Env();

    if (!info[0].IsNumber()) {
        return CreateErrorObject(env, 1, "Expected number as first argument.");
    }

    int32_t encoderId = info[0].As<Number>().Int32Value();
    string error = XdeltaEncodeAsyncWorker::RequestCancellation(encoderId);
    if (!error.empty()) {
        return CreateErrorObject(env, 2, error);
    }

    return env.Undefined();
}

Object Init(Env env, Object exports) {

    Object xd3SmatchConfig = Object::New(env);
    xd3SmatchConfig.Set("default", (int)XD3_SMATCH_DEFAULT);
    xd3SmatchConfig.Set("slow", (int)XD3_SMATCH_SLOW);
    xd3SmatchConfig.Set("fast", (int)XD3_SMATCH_FAST);
    xd3SmatchConfig.Set("faster", (int)XD3_SMATCH_FASTER);
    xd3SmatchConfig.Set("fastest", (int)XD3_SMATCH_FASTEST);
    xd3SmatchConfig.Set("soft", (int)XD3_SMATCH_SOFT);

    exports.Set("xd3SmatchConfig", xd3SmatchConfig);

    exports.Set(String::New(env, "EncodeBuffer"),
                Function::New(env, EncodeBuffer));
    exports.Set(String::New(env, "DecodeBuffer"),
                Function::New(env, DecodeBuffer));
    exports.Set(String::New(env, "CancelEncode"),
                Function::New(env, CancelEncode));
    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
