#include <napi.h>

#include <xdelta3.h>

#include <XdeltaEncodeAsyncWorker.h>

using namespace Napi;

XdeltaEncodeAsyncWorker::XdeltaEncodeAsyncWorker(Buffer<uint8_t> &dictionary,
                                                Buffer<uint8_t> &target,
                                                Buffer<uint8_t> &result,
                                                Function& callback) : BaseXdeltaAsyncWorker(dictionary,
                                                                                            target,
                                                                                            result,
                                                                                            callback)
{
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
                            0);
}
