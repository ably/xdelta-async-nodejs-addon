#include <map>

#include <napi.h>

#include <xdelta3.h>

#include <XdeltaEncodeAsyncWorker.h>

using namespace std;
using namespace Napi;

uint16_t XdeltaEncodeAsyncWorker::maxSimultaneouslyRunningEncoders = 255;
map<uint16_t, XdeltaEncodeAsyncWorker *> XdeltaEncodeAsyncWorker::runningEncoders = map<uint16_t, XdeltaEncodeAsyncWorker *>();

XdeltaEncodeAsyncWorker::XdeltaEncodeAsyncWorker(uint16_t id,
                                                Buffer<uint8_t> &dictionary,
                                                Buffer<uint8_t> &target,
                                                Buffer<uint8_t> &result,
                                                Function& callback) : BaseXdeltaAsyncWorker(dictionary,
                                                                                            target,
                                                                                            result,
                                                                                            callback),
                                                                    id(id),
                                                                    cancellationRequested(0)
{
}

XdeltaEncodeAsyncWorker::~XdeltaEncodeAsyncWorker()
{
    XdeltaEncodeAsyncWorker::runningEncoders.erase(id);
}

void XdeltaEncodeAsyncWorker::Execute()
{
    res = xd3_encode_memory(inputPtr,
                            inputLength,
                            dictionaryPtr,
                            dictionaryLength,
                            resultPtr,
                            &bytesWritten,
                            resultLength,
                            0,
                            &cancellationRequested);
}

void XdeltaEncodeAsyncWorker::RequestCancellation()
{
    cancellationRequested = 1;
}

uint16_t XdeltaEncodeAsyncWorker::GetNewEncoderId() {
    static uint16_t nextEncoderId = 0;
    uint16_t encoderId = nextEncoderId;
    nextEncoderId = (encoderId + 1) % XdeltaEncodeAsyncWorker::maxSimultaneouslyRunningEncoders;
    return encoderId;
}

string XdeltaEncodeAsyncWorker::New(Buffer<uint8_t> &dictionary,
                                    Buffer<uint8_t> &target,
                                    Buffer<uint8_t> &result,
                                    Function& callback,
                                    XdeltaEncodeAsyncWorker **encoder)
{
    uint64_t encoderId = GetNewEncoderId();
    if (XdeltaEncodeAsyncWorker::runningEncoders.find(encoderId) != XdeltaEncodeAsyncWorker::runningEncoders.end()) {
        return "Max number of simultaneously running encoders exceeded.";
    }

    *encoder = new XdeltaEncodeAsyncWorker(encoderId, dictionary, target, result, callback);
    XdeltaEncodeAsyncWorker::runningEncoders[encoderId] = *encoder;
    return "";
}

string XdeltaEncodeAsyncWorker::RequestCancellation(int32_t id)
{
    if (XdeltaEncodeAsyncWorker::runningEncoders.find(id) == XdeltaEncodeAsyncWorker::runningEncoders.end()) {
        return "Encoder not found.";
    }

    XdeltaEncodeAsyncWorker::runningEncoders[id]->RequestCancellation();
    return "";
}

string XdeltaEncodeAsyncWorker::SetMaxSimultaneouslyRunningEncoders(int32_t requestedMaxSimultaneouslyRunningEncoders)
{
    if (requestedMaxSimultaneouslyRunningEncoders <= 0 ||
        requestedMaxSimultaneouslyRunningEncoders > 0xFFFF)
    {
        return "Max number of simultaneously running encoders should be between 1 and 65535.";
    }

    XdeltaEncodeAsyncWorker::maxSimultaneouslyRunningEncoders = (uint16_t)requestedMaxSimultaneouslyRunningEncoders;
    return "";
}
