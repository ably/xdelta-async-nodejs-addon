#include <map>

#include <napi.h>

#include <xdelta3.h>

#include <XdeltaEncodeAsyncWorker.h>

using namespace std;
using namespace Napi;

// https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number/MAX_SAFE_INTEGER
#define JS_MAX_SAFE_INTEGER 0x1FFFFFFFFFFFFF

map<int64_t, XdeltaEncodeAsyncWorker *> XdeltaEncodeAsyncWorker::runningEncoders = map<int64_t, XdeltaEncodeAsyncWorker *>();

XdeltaEncodeAsyncWorker::XdeltaEncodeAsyncWorker(int64_t id,
                                                Buffer<uint8_t> &dictionary,
                                                Buffer<uint8_t> &target,
                                                Buffer<uint8_t> &result,
                                                int32_t stringMatcherType,
                                                Function& callback) : BaseXdeltaAsyncWorker(dictionary,
                                                                                            target,
                                                                                            result,
                                                                                            callback),
                                                                    id(id),
                                                                    cancellationRequested(0),
                                                                    stringMatcherType(stringMatcherType)
{
}

XdeltaEncodeAsyncWorker::~XdeltaEncodeAsyncWorker()
{
    XdeltaEncodeAsyncWorker::runningEncoders.erase(id);
}

void XdeltaEncodeAsyncWorker::Execute()
{
    res = xd3_encode_memory_enhanced(inputPtr,
                            inputLength,
                            dictionaryPtr,
                            dictionaryLength,
                            resultPtr,
                            &bytesWritten,
                            resultLength,
                            0,
                            &cancellationRequested,
                            (xd3_smatch_cfg)stringMatcherType);
}

void XdeltaEncodeAsyncWorker::RequestCancellation()
{
    cancellationRequested = 1;
}

int64_t XdeltaEncodeAsyncWorker::GetNewEncoderId() {
    static int64_t nextEncoderId = 0;
    int64_t encoderId = nextEncoderId;
    nextEncoderId = (encoderId + 1) % JS_MAX_SAFE_INTEGER;
    return encoderId;
}

string XdeltaEncodeAsyncWorker::New(Buffer<uint8_t> &dictionary,
                                    Buffer<uint8_t> &target,
                                    Buffer<uint8_t> &result,
                                    int32_t stringMatcherType,
                                    Function& callback,
                                    XdeltaEncodeAsyncWorker **encoder)
{
    int64_t encoderId = GetNewEncoderId();
    *encoder = new XdeltaEncodeAsyncWorker(encoderId, dictionary, target, result, stringMatcherType, callback);
    XdeltaEncodeAsyncWorker::runningEncoders[encoderId] = *encoder;
    return "";
}

string XdeltaEncodeAsyncWorker::RequestCancellation(int64_t id)
{
    if (XdeltaEncodeAsyncWorker::runningEncoders.find(id) == XdeltaEncodeAsyncWorker::runningEncoders.end()) {
        return "Encoder not found.";
    }

    XdeltaEncodeAsyncWorker::runningEncoders[id]->RequestCancellation();
    return "";
}
